#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <numeric>
#include <math.h>
#include <algorithm>
#include <jansson.h>
#include <curl/curl.h>
#include <iostream>

#include "OrderManager.H"
#include "UserSetting.H"
#include "base64.H"
#include "result.H"
#include "time_fun.H"
#include "curl_fun.H"
#include "parameters.H"
#include "send_email.H"
#include "OKCoinChina.H"

int main(int argc, char **argv) {

    std::cout << "Vulture Arbitrage\n" << std::endl;
    std::cout << "DISCLAIMER: USE THE SOFTWARE AT YOUR OWN RISK.\n" << std::endl;

    // read the config file (config.json)
    json_error_t error;
    json_t *root = json_load_file("config.json", 0, &error);
    UserSetting userSettings;
    OKCoinChina okCoinChina;

    if (!root) {
        std::cout << "ERROR: config.json incorrect (" << error.text << ")\n" << std::endl;
        return 1;
    }
    if(userSettings.initialize(root)) {
        std::cout << "Config values have been passed successfully" << std::endl;
    }
    OrderManager orderMgr(okCoinChina,userSettings);
    if(!orderMgr.initialize())
    {
	std::cout << "Error to initialize OrderMgr. Exit.." << std::endl;
	return -1;
    }
    orderMgr.start();
    return 0;
}
