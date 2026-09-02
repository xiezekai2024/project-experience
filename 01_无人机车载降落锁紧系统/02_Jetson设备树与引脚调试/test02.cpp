#include <gpiod.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>

bool running = true;

void handle_sigint(int){
    running = false;
}

int main() {
    signal(SIGINT, handle_sigint);

    const char* chipname = "gpiochip0";
    int line_offset = 105;

    gpiod_chip* chip = gpiod_chip_open_by_name(chipname);
    if (!chip){
        std::cout << "Failed to open chip\n";
        return -1;
    }

    gpiod_line* line = gpiod_chip_get_line(chip, line_offset);
    if (!line){
        std::cout << "Failed to get line\n";
        gpiod_chip_close(chip);
        return -1;
    }

    if (gpiod_line_request_output(line, "cppgpio", 0) < 0){
        std::cout << "Failed to request line as output\n";
        gpiod_chip_close(chip);
        return -1;
    }

    std::cout << "Blinking GPIO " << line_offset << "...\n";

    while (running){
        gpiod_line_set_value(line, 1);
        std::cout << "1\n";
        sleep(1);

        gpiod_line_set_value(line, 0);
        std::cout << "0\n";
        sleep(1);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
