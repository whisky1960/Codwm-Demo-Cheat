/*
aarch64-linux-android34-clang++ main.cpp utils.cpp -o codwm-cheat-demo -static
adb push codwm-cheat-demo /data/local/tmp
adb shell su -c chmod +x /data/local/tmp/codwm-cheat-demo
adb shell su -c /data/local/tmp/codwm-cheat-demo
*/
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"
#include "offsets.h"

#define AIM_INDEX 82

pid_t pid;
uintptr_t game_base;
uintptr_t client_info;
Vector3 local_camera_pos;
// Your screen width and height. You can get them by run `wm size`
float screen_x = 2400;
float screen_y = 1080;

iw_refdef_t refdef;

bool world_to_screen(const Vector3& WorldPos, Vector2* ScreenPos) {
  Vector3 ViewOrig = local_camera_pos;
  Vector3 vLocal, vTransform;
  vLocal = WorldPos - ViewOrig;
  // get our dot products from viewAxis
  vTransform.x = vLocal.Dot(refdef.ViewAxis[1]);
  vTransform.y = vLocal.Dot(refdef.ViewAxis[2]);
  vTransform.z = vLocal.Dot(refdef.ViewAxis[0]);
  // make sure it is in front of us
  if (vTransform.z < 0.01f)
    return false;

  auto scale = Vector2(float(screen_x) / float(refdef.Width), float(screen_y) / float(refdef.Height));
  ScreenPos->x = ((float(refdef.Width) / 2.f) * (1 - (vTransform.x / refdef.FovX / vTransform.z))) * scale.x;
  ScreenPos->y = ((float(refdef.Height) / 2.f) * (1 - (vTransform.y / refdef.FovY / vTransform.z))) * scale.y;
  return true;
}

Vector2 calculate_angle(Vector3 src, Vector3 dst) {
  Vector2 angles = Vector2(0, 0);

  Vector3 delta = src - dst;
  float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

  angles.x = atanf(delta.x / delta.y) * 57.2957795131f;
  angles.y = atanf(-delta.z / hyp) * -57.2957795131f;

  if (delta.y >= 0.0f)
    angles.x -= 180.0f;

  return angles;
}

Vector2 get_viewangle() {
  Vector2 viewangle = Vector2(0, 0);
  viewangle.y = read<float>(pid, client_info + OFF_PLAYER_VIEW_ANGLE);
  viewangle.x = 360.f - read<float>(pid, client_info + OFF_PLAYER_VIEW_ANGLE + 4);
  if (viewangle.y > 85.0f)
    viewangle.y = viewangle.y - 360.0f;
  viewangle.x -= 270.f;

  return viewangle;
}

Vector3 get_aim_bone_pos(int index) {
  auto bone_index = read<uint16_t>(pid, game_base + OFF_BONE_INDEX + index * 2);
  auto bone_base_ptr = read<uintptr_t>(pid, game_base + OFF_BONE_PTR);
  if (!bone_base_ptr) {
    return Vector3{};
  }
  auto bone_ptr = read<uintptr_t>(pid, bone_base_ptr + bone_index * SIZE_BONE + 0xD8);
  if (!bone_ptr) {
    return Vector3{};
  }
  auto bone_pos = read<Vector3>(pid, bone_ptr + (BONE_POS_HEAD * 0x20) + 0x10);
  auto base_bone_pos = read<Vector3>(pid, client_info + OFF_BONE_BASE);
  auto aim_bone_pos = bone_pos + base_bone_pos;
  return aim_bone_pos;
}

union encrypted_float {
  uint32_t u32;
  float f;
};

encrypted_float decrypt(uint64_t pointer, encrypted_float value, uint32_t key) {
  encrypted_float result;
  result.u32 = (((key ^ pointer) + 2) * (key ^ pointer)) ^ value.u32;
  return result;
}


int main() {
  pid = pidof("com.activision.callofduty.warzone");
  game_base = get_module_base_by_name_and_length(pid, "split_config.arm64_v8a.apk", 0xe57000);
  printf("[+] game_base: %#lx\n", game_base);

  client_info = read<uintptr_t>(pid, game_base + OFF_CLIENT_INFO);
  printf("[+] client_info: %#lx\n", client_info);
  if (!client_info) {
    return -1;
  }
  auto local_player_index = read<uint32_t>(pid, client_info + OFF_CURRENT_INDEX);
  auto player_entity_base = read<uintptr_t>(pid, client_info + OFF_PLAYER_ENTITY);
  printf("[+] local_player_index: %d\n", local_player_index);
  printf("[+] player_entity_base: %#lx\n", player_entity_base);
  if (!player_entity_base) {
    return -1;
  }
  auto local_player_entity = player_entity_base + SIZE_PLAYER_ENTITY * local_player_index;
  auto local_player_teamid = read<uint8_t>(pid, local_player_entity + OFF_PLAYER_TEAMID);
  auto local_player_pos_ptr = read<uintptr_t>(pid, local_player_entity + OFF_PLAYER_POS_PTR);
  if (!local_player_pos_ptr) {
    printf("l[-] ocal_player_pos_ptr: %#lx\n", local_player_pos_ptr);
    return -1;
  }
  auto local_player_pos = read<Vector3>(pid, local_player_pos_ptr + OFF_PLAYER_POS);
  printf("[+] local_player_pos: %.2f %.2f %.2f\n", local_player_pos.x, local_player_pos.y, local_player_pos.z);

  auto name_base = read<uintptr_t>(pid, game_base + OFF_NAME_BASE);
  printf("[+] name_base: %#lx\n", name_base);

  auto local_player_name_t = read<iw_entine_name_t>(pid, name_base + OFF_NAME_INFO + sizeof(iw_entine_name_t) * local_player_index);
  printf("[+] name: %s\n", local_player_name_t.name);
  printf("[+] health: %d\n", local_player_name_t.health);

  auto local_camera_ptr = read<uintptr_t>(pid, game_base + OFF_CAMERA_PTR);
  local_camera_pos = read<Vector3>(pid, local_camera_ptr + OFF_CAMERA_PTR_POS);
  auto refdef_ptr = read<uintptr_t>(pid, game_base + OFF_REFDEF_PTR);
  refdef = read<iw_refdef_t>(pid, refdef_ptr);
  printf("[+] refdef: \n"
          "      width: %d\n"
          "      height: %d\n"
          "      FovX: %.2f\n"
          "      FovY: %.2f\n"
          "      ViewAxis: %.2f %.2f\n", 
    refdef.Width, refdef.Height, refdef.FovX, refdef.FovY, refdef.ViewAxis[0].x, refdef.ViewAxis[0].y);
  printf("[+] local_camera_pos: %.2f %.2f %.2f\n", local_camera_pos.x, local_camera_pos.y, local_camera_pos.z);

  puts("");
  printf("[*] players\n");
  for (int i = 0; i < 120; i++) {
    auto player_entity = player_entity_base + SIZE_PLAYER_ENTITY * i;
    auto player_pos_ptr = read<uintptr_t>(pid, player_entity + OFF_PLAYER_POS_PTR);
    if (!player_pos_ptr || player_pos_ptr > 0x8000000000 || player_pos_ptr < 0x1000000000) {
      continue;
    }
    auto player_pos = read<Vector3>(pid, player_pos_ptr + OFF_PLAYER_POS);
    if (player_pos.x == 0 && player_pos.y == 0 && player_pos.z == 0) {
      continue;
    }
    auto player_name_t = read<iw_entine_name_t>(pid, name_base + OFF_NAME_INFO + sizeof(iw_entine_name_t) * i);
    Vector2 screen_pos;
    if (world_to_screen(player_pos, &screen_pos) == false) {
      continue;
    }
    printf("[+] player %3d: %20s\t %3d %10.2f %10.2f %10.2f     %10.2f %10.2f\n", i, player_name_t.name, player_name_t.health, player_pos.x, player_pos.y, player_pos.z, screen_pos.x, screen_pos.y);
  }

  puts("");
  auto viewangle_ptr = read<uintptr_t>(pid, game_base + OFF_PLAYER_VIEW_ANGLE_CI);
  if (!viewangle_ptr) {
    return -1;
  }
  
  printf("[*] view\n");    
  auto viewangle_key = read<uint32_t>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_KEY);
  // auto viewangle_key2 = read<uint32_t>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_KEY2);
  printf("[+] viewangle_key: 0x%08x\n", viewangle_key);
  
  encrypted_float viewangle_y = read<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Y);
  encrypted_float viewangle_x = read<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_X);
  encrypted_float viewangle_z = read<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Z);
  encrypted_float origin_viewangle_y = viewangle_y;
  encrypted_float origin_viewangle_x = viewangle_x;
  encrypted_float origin_viewangle_z = viewangle_z;
  printf("[+] viewangle_y: 0x%08x %.2f\n", viewangle_y.u32, viewangle_y.f);
  printf("[+] viewangle_x: 0x%08x %.2f\n", viewangle_x.u32, viewangle_x.f);
  printf("[+] viewangle_z: 0x%08x %.2f\n", viewangle_z.u32, viewangle_z.f);

  printf("[*] decrypt\n");    
  viewangle_y = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Y, viewangle_y, viewangle_key);
  viewangle_x = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_X, viewangle_x, viewangle_key);
  viewangle_z = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Z, viewangle_z, viewangle_key);
  printf("[+] viewangle_y: 0x%08x %.2f\n", viewangle_y.u32, viewangle_y.f);
  printf("[+] viewangle_x: 0x%08x %.2f\n", viewangle_x.u32, viewangle_x.f);
  printf("[+] viewangle_z: 0x%08x %.2f\n", viewangle_z.u32, viewangle_z.f);

  // aim at someone 
  auto aim_index = AIM_INDEX;
  auto aim_entity = player_entity_base + SIZE_PLAYER_ENTITY * aim_index;
  auto aim_pos_ptr = read<uintptr_t>(pid, aim_entity + OFF_PLAYER_POS_PTR);
  auto aim_entity_pos = read<Vector3>(pid, aim_pos_ptr + OFF_PLAYER_POS);
  aim_entity_pos.z += 50.f;
  auto aim_bone_pos = get_aim_bone_pos(aim_index);
  Vector3 aim_pos{};
  // sometimes the aim_bone_pos is wrong, we use aim_entity_pos
  if (aim_bone_pos.IsZero() || aim_bone_pos.distance_to(aim_entity_pos) >= 100) {
      aim_pos = aim_entity_pos;
  } else {
      aim_pos = aim_bone_pos;
  }
  printf("[+] aim at player %d: %.2f %.2f %.2f\n", aim_index, aim_pos.x, aim_pos.y, aim_pos.z);
  Vector2 delta_angle = calculate_angle(local_camera_pos, aim_pos);
  Vector2 viewangle_ci = get_viewangle();
  viewangle_y.f += (delta_angle.y - viewangle_ci.y);
  viewangle_x.f -= (delta_angle.x - viewangle_ci.x);
  printf("[+] viewangle_y: 0x%08x %.2f\n", viewangle_y.u32, viewangle_y.f);
  printf("[+] viewangle_x: 0x%08x %.2f\n", viewangle_x.u32, viewangle_x.f);
  printf("[+] viewangle_z: 0x%08x %.2f\n", viewangle_z.u32, viewangle_z.f);

  printf("[*] encrypt\n");
  viewangle_y = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Y, viewangle_y, viewangle_key);
  viewangle_x = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_X, viewangle_x, viewangle_key);
  viewangle_z = decrypt(viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Z, viewangle_z, viewangle_key);
  printf("[+] viewangle_y: 0x%08x %.2f\n", viewangle_y.u32, viewangle_y.f);
  printf("[+] viewangle_x: 0x%08x %.2f\n", viewangle_x.u32, viewangle_x.f);
  printf("[+] viewangle_z: 0x%08x %.2f\n", viewangle_z.u32, viewangle_z.f);
  
  printf("[*] write view angle\n");
  write<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Y, viewangle_y);
  write<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_X, viewangle_x);
  write<encrypted_float>(pid, viewangle_ptr + OFF_PLAYER_VIEW_ANGLE_Z, viewangle_z);

  puts("");
  printf("[*] no recoil\n");
  // process_vm_writev will failed, so we use /proc/pid/mem here
  auto recoil_func = game_base + OFF_PLAYER_RECOIL;
  char buf[100];
  snprintf(buf, 100, "/proc/%d/mem", pid);
  int fd = open(buf, O_RDWR);
  lseek(fd, recoil_func, SEEK_SET);
  auto res = write(fd, "\xC0\x03\x5F\xD6", 4); // ret
  if (res != 4) {
      printf("[-] write failed\n");
      close(fd);
      return -1;
  }
  close(fd);
  printf("[+] patch recoil success\n");
}
