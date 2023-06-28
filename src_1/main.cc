// Copyright(C) Gaussian Robot. All rights reserved.
//
// @file clean_mode_controller.h
// @brief control device
// @author Sun Shichao<sunshichao@gs-robot.com>
// @version 1.0.0.0
// @date 2018-06-15
//

#include "common/common_inc.h"  // NOLINT
#include "device_controller.h"  // NOLINT

#include "common/ros/ros_platform.h"
#include "ros_device_controller.h"  // NOLINT
#include "common/people.h"
using namespace std;

string ori = R"(
{
  "Tea_1": {
    "name": "Tea_1",
    "subject": 3,
    "stus": [
      {
        "id": 333,
        "sex": false,
        "score": 80,
        "name": "Stu.a"
      },
      {
        "id": 444,
        "sex": true,
        "score": 0,
        "name": "Stu.b"
      }
    ]
  },
  "Tea_2": {
    "name": "Tea_2",
    "subject": 1,
    "stus": [
      {
        "id": 555,
        "sex": true,
        "score": 59,
        "name": "Stu.c"
      }
    ]
  }
}
)";

int main(int argc, char** argv) {
  map<string, common::Teacher> tchs;

  // 从json字符串反序列化对象
  bool ret = common::Unmarshal(tchs, ori);
  if (!ret) {
    cout << "反序列失败" << endl;
    return 0;
  } else {
    cout << "反序列成功" << endl << tchs["Tea_2"].stus[0].id;
  }

  RosPlatform platform(argc, argv, "gs_device_controller");
  platform.SetModule("device_controller", new RosDeviceController(&platform));
  platform.Initialize();
  platform.Startup();
  platform.Run();
  platform.Cleanup();
  platform.UnInitialize();

  return 0;
}

