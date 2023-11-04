#include <stdio.h>
#include <string.h>
#include "unity.h"

static void print_banner(const char* text);

void app_main(void){
    print_banner("Executing unit tests for BMP280_I2C component");
    UNITY_BEGIN();
    unity_run_tests_by_tag("BMP280_I2C", false);
    UNITY_END();

    print_banner("Executing unit tests for Interrupts component");
    UNITY_BEGIN();
    unity_run_tests_by_tag("Interrupts", false);
    UNITY_END();

    print_banner("Running unit tests for LcdDisplay component");
    UNITY_BEGIN();
    unity_run_tests_by_tag("LcdDisplay", false);
    UNITY_END();
}

static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}

