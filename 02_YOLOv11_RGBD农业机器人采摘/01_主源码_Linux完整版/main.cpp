#include "algo.h"
#include "kinect_camera.h"
#include "tcp_comm.h"
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <memory>

int main(int argc, char* argv[]) {
    std::string ip = argc > 1 ? argv[1] : "127.0.0.1";
	int port = argc > 2 ? std::stoi(argv[2]) : 8088;
	std::string model_path = argc > 3 ? argv[3] : "a3.onnx";
		    
	std::cout << "Starting TCP Communication Test" << std::endl;
	std::cout << "Server: " << ip << ":" << port << std::endl;
	
	// 实例化一个对象，使用有参构造，用于连接机械臂尝试
	TCPClient client(ip, port);
	
	// connection_flag参数默认为false，只有连接成功才会变成true
	if(!client.connection_flag){
	    return -1;
	}

    // 创建并初始化Kinect相机
	// 创建一个KinectCamera的智能指针，对其进行初始化操作
    std::unique_ptr<KinectCamera> kinect = std::make_unique<KinectCamera>();
	// 初始化相机参数
    if (!kinect->initialize()) {
        return -1;
    }

    Algo algo;
    algo.setModePath(model_path);
    algo.setStride(20);
    algo.init();
    
    cv::Mat frame;
    cv::Mat raw_frame;
    cv::Mat depth_color;
    cv::Mat mask;
    cv::Mat aligned_depth;        // 对齐后的深度数据（16位）
    cv::Mat aligned_depth_color;  // 对齐后的深度可视化
    cv::Mat blended;
    cv::Rect resultBox;  //检测目标框
    cv::Point3f point2d;
    cv::Point3f point3d;
    Eigen::Vector4f centroid;
    
    cv::Point3f point3d_arm;
    Eigen::Vector3f euler_angles_arm;
    cv::Mat H_hand_eye = (cv::Mat_<double>(4, 4) << 
        0.13551828522285903,  0.99077427969707599,  0.0010588016795823466, -135.91254527050771,
        -0.99077376072521717,  0.13551964081471257, -0.0013349201705961042,  -51.733981386034912,
        -0.0014660929937864665, -0.0008681268295133869, 0.99999854846251723,  -90.646138164038007,
        0.0, 0.0, 0.0, 1.0);  
    cv::Mat axis_remap = (cv::Mat_<double>(4,4) << 
        0,  0,  1,  0,
        1,  0,  0,  0,
        0, 1,  0,  0,
        0,  0,  0,  1);
    //H_hand_eye = H_hand_eye * axis_remap;

    // 检查矩阵是否正确初始化
    std::cout << "Hand-eye calibration matrix:\n" << H_hand_eye << std::endl;
    
    
    while (true) {
        // 从Kinect获取帧（包含对齐的深度图）
        if (kinect->getNextFrame(frame, depth_color, &aligned_depth, &aligned_depth_color)) {
            // 显示原始深度图
            //cv::imshow("Original Depth", depth_color);
            // 显示对齐后的深度图
            //cv::imshow("Aligned Depth", aligned_depth_color);
            
            if (frame.empty()) continue;
            raw_frame = frame.clone();
            
            // 使用对齐的深度数据进行处理，在这里，frame 和 aligned_depth 已经对齐
            if (!aligned_depth.empty()) {
                // 可以基于检测结果获取深度值
                bool res = algo.inferResult(frame, resultBox);
                if(!res){
                       std::cout<<"None Detection!!!"<<std::endl;
                	continue;
                }
                
                cv::Point keypoint = algo.calculateCentroid(raw_frame, resultBox, mask);

                aligned_depth.setTo(0, ~mask);

                cv::resize(aligned_depth_color,aligned_depth_color,frame.size());
                //cv::imshow("Aligned Depth", aligned_depth_color);
                
                cv::addWeighted(frame,0.7,aligned_depth_color,0.3,0,blended);   
                
                //可视化
                cv::resize(frame, frame, cv::Size(640, 480));
                //cv::imshow("Detection", frame);
                //cv::resize(aligned_depth_color,aligned_depth_color,frame.size());
                //cv::imshow("Aligned Depth", aligned_depth_color);
                cv::resize(blended,blended,frame.size());
                cv::imshow("Blended View", blended);
                //cv::resize(raw_frame,raw_frame,frame.size());
                //cv::imshow("raw frame",raw_frame);
                cv::waitKey(1000);
		 
			    //检测框中心
			    cv::Point center(resultBox.x + resultBox.width/2, resultBox.y + resultBox.height/2);
			    cv::circle(frame, center, 10, cv::Scalar(255, 0, 0), -1);
			 
			    //目标质心
			    std::cout<<"pointx="<<keypoint.x<<" pointy="<<keypoint.y<<std::endl;
			    cv::circle(frame, keypoint, 10, cv::Scalar(0, 0, 255), -1);
		 
				// 获取中心点深度值
				if (keypoint.x >= 0 && keypoint.x < aligned_depth.cols &&
				    keypoint.y >= 0 && keypoint.y < aligned_depth.rows) 
				{
				    uint16_t depth_value = aligned_depth.at<uint16_t>(keypoint);
				    
				    if (depth_value > 0) {
				        // 转换为3D坐标（彩色相机坐标系）
				        int valid = 0;
				        point2d = kinect->pixelToPoint3D(keypoint, depth_value, &valid);

				        //point3d.z /= 1000.0f ;
				        
				        std::cout << "2D质心("
				                      << point2d.x << ", " 
				                      << point2d.y << ", "
				                      << point2d.z << ") mm" << std::endl;
				    }
				}
        
        		pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

				if (kinect->depthImageToPointCloud(aligned_depth, cloud, raw_frame)) {
				           
				    //过滤点云，提取最大簇(这个部分存在 段错误 (核心已转储) 的问题)
				    /*pcl::PointCloud<pcl::PointXYZRGB>::Ptr largest_cluster = kinect->extractLargestEuclideanCluster(cloud, 0.01, 50, 100000);*/
				    pcl::PointCloud<pcl::PointXYZRGB>::Ptr largest_cluster = cloud;
					
					//默认于相机姿态相同
				    Eigen::Vector3f euler_angles(0, 0, 0);  
				    std::cout<<"start"<<std::endl;		    
					
					//使用PCA取得目标点云的旋转姿态
					/*euler_angles = algo.computeOrientationFromPointCloud(largest_cluster);
					std::cout << "Orientation (Yaw, Pitch, Roll): "
				                      << euler_angles.transpose() << "rad" << std::endl;*/
					
					//取得质心
					pcl::compute3DCentroid(*largest_cluster, centroid);
					point3d.x = centroid.x();
					point3d.y = centroid.y();
					point3d.z = centroid.z();
					std::cout << "6D位姿Cam("<< point3d.x << ", " 
				                     << point3d.y << ", "
				                     << point3d.z << ") m, " 
				                     << euler_angles[0] << ", "
				                     << euler_angles[1] << ", "
				                     << euler_angles[2] << "rad"
				                     << std::endl;
				                     
				    algo.transformHandEye(H_hand_eye,point3d*1000,euler_angles,point3d_arm,euler_angles_arm);
				    point3d_arm /= 1000;
				    std::cout << "6D位姿Arm("<< point3d_arm.x << ", " 
				                     << point3d_arm.y << ", "
				                     << point3d_arm.z << ") m, " 
				                     << euler_angles_arm[0] << ", "
				                     << euler_angles_arm[1] << ", "
				                     << euler_angles_arm[2] << "rad"
				                     << std::endl;
				                     
				           
					client.sendPose0(point3d_arm,euler_angles_arm);
					int flag = 5;
					while(true) {
						if (client.waitForMsg("Finish")) {
							std::cout << "Successfully received Finish!" << std::endl;
							break;
						} else {
							std::cerr << "Failed to receive Finish, Waiting..." << std::endl;
							std::this_thread::sleep_for(std::chrono::seconds(1));
							
						}
					}
				                      
					// 可视化点云
					/*kinect->visualizePointCloud(
						largest_cluster, 
						"PointCloud with Pose",
						cv::Scalar(0, 0, 0), 
						point3d,                    
						euler_angles,                     
						0.10f,
						"camera_pose"); */           
				}              
            }

            // 按 ESC 退出
            if (cv::waitKey(30) == 27) break;
        } else {
            std::cout << "Frame capture failed" << std::endl;
            continue;
        }
    }
    
    kinect->shutdown();
    cv::destroyAllWindows();
    return 0;
}


