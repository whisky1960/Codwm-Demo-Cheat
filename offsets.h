#pragma once
#include "utils.h"

// client_info
#define OFF_CLIENT_INFO             0xE62CE58
#define OFF_CURRENT_INDEX           0x724F8
#define OFF_PLAYER_ENTITY           0x18C198
#define SIZE_PLAYER_ENTITY          0x13CA8
#define OFF_PLAYER_TEAMID           0x13AD8
#define OFF_PLAYER_POS_PTR          0x12EA8
#define OFF_PLAYER_POS              0x48
#define OFF_PLAYER_VIEW_ANGLE       0x14C95C
#define OFF_PLAYER_VIEW_ANGLE_CI    0xB538980
#define OFF_PLAYER_VIEW_ANGLE_KEY   0x13108
#define OFF_PLAYER_VIEW_ANGLE_Y     0x13128
#define OFF_PLAYER_VIEW_ANGLE_X     OFF_PLAYER_VIEW_ANGLE_Y + 4
#define OFF_PLAYER_VIEW_ANGLE_Z     OFF_PLAYER_VIEW_ANGLE_Y + 8
#define OFF_NAME_BASE               0xE63C660
#define OFF_NAME_INFO               0x5E80
#define OFF_CAMERA_PTR              0xDE20E80
#define OFF_CAMERA_PTR_POS          0x218
#define OFF_REFDEF_PTR              0xE630440
#define OFF_BONE_PTR                0x630B3F0
#define SIZE_BONE                   0x178
#define OFF_BONE_BASE               0x164428
#define OFF_BONE_INDEX              0x630B3F8
#define OFF_PLAYER_RECOIL           0x416A388

// need to update
enum BONE_INDEX : unsigned long {
  // BONE_POS_HELMET = 8,

  BONE_POS_HEAD = 69,
  // BONE_POS_NECK = 6,
  // BONE_POS_CHEST = 5,
  // BONE_POS_MID = 4,
  // BONE_POS_TUMMY = 3,
  // BONE_POS_PELVIS = 2,

  // BONE_POS_RIGHT_FOOT_1 = 21,
  // BONE_POS_RIGHT_FOOT_2 = 22,
  // BONE_POS_RIGHT_FOOT_3 = 23,
  // BONE_POS_RIGHT_FOOT_4 = 24,

  // BONE_POS_LEFT_FOOT_1 = 17,
  // BONE_POS_LEFT_FOOT_2 = 18,
  // BONE_POS_LEFT_FOOT_3 = 19,
  // BONE_POS_LEFT_FOOT_4 = 20,

  // BONE_POS_LEFT_HAND_1 = 13,
  // BONE_POS_LEFT_HAND_2 = 14,
  // BONE_POS_LEFT_HAND_3 = 15,
  // BONE_POS_LEFT_HAND_4 = 16,

  // BONE_POS_RIGHT_HAND_1 = 9,
  // BONE_POS_RIGHT_HAND_2 = 10,
  // BONE_POS_RIGHT_HAND_3 = 11,
  // BONE_POS_RIGHT_HAND_4 = 12
};

struct iw_entine_name_t {
  unsigned int index;
  char name[36];
  unsigned char pad[92];
  unsigned int health;
  unsigned char pad2[78];
};

struct iw_refdef_t {
  char pad_0000[8]{};
  unsigned int Width{};
  unsigned int Height{};
  float FovX{};
  float FovY{};
  float Unk{};
  char pad_001C[8]{};
  Vector3 ViewAxis[3];
};
