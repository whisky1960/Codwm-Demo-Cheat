#define __USE_GNU

#include <cstdlib>
#include <sys/types.h>
#include <sys/uio.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "offsets.h"
#include "utils.h"

Vector3 Vector3::operator-(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x - rhs.x;
  ret.y = this->y - rhs.y;
  ret.z = this->z - rhs.z;
  return ret;
}

Vector3 Vector3::operator+(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x + rhs.x;
  ret.y = this->y + rhs.y;
  ret.z = this->z + rhs.z;
  return ret;
}

Vector3 Vector3::operator*(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x * rhs.x;
  ret.y = this->y * rhs.y;
  ret.z = this->z * rhs.z;
  return ret;
}

Vector3 Vector3::operator/(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x / rhs.x;
  ret.y = this->y / rhs.y;
  ret.z = this->z / rhs.z;
  return ret;
}

Vector3 Vector3::operator-=(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x;
  ret.y = this->y;
  ret.z = this->z;

  ret.x -= rhs.x;
  ret.y -= rhs.y;
  ret.z -= rhs.z;

  return ret;
}

Vector3 Vector3::operator+=(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x;
  ret.y = this->y;
  ret.z = this->z;

  ret.x += rhs.x;
  ret.y += rhs.y;
  ret.z += rhs.z;

  return ret;
}

Vector3 Vector3::operator*=(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x;
  ret.y = this->y;
  ret.z = this->z;

  ret.x *= rhs.x;
  ret.y *= rhs.y;
  ret.z *= rhs.z;

  return ret;
}

Vector3 Vector3::operator/=(const Vector3& rhs) const {
  Vector3 ret;
  ret.x = this->x;
  ret.y = this->y;
  ret.z = this->z;

  ret.x /= rhs.x;
  ret.y /= rhs.y;
  ret.z /= rhs.z;

  return ret;
}

Vector3 Vector3::operator+(const float& rhs) const {
  Vector3 ret;
  ret.x = this->x + rhs;
  ret.y = this->y + rhs;
  ret.z = this->z + rhs;

  return ret;
}

Vector3 Vector3::operator-(const float& rhs) const {
  Vector3 ret;
  ret.x = this->x - rhs;
  ret.y = this->y - rhs;
  ret.z = this->z - rhs;

  return ret;
}

Vector3 Vector3::operator/(const float& rhs) const {
  Vector3 ret;
  ret.x = this->x / rhs;
  ret.y = this->y / rhs;
  ret.z = this->z / rhs;

  return ret;
}

Vector3 Vector3::operator*(const float& rhs) const {
  Vector3 ret;
  ret.x = this->x * rhs;
  ret.y = this->y * rhs;
  ret.z = this->z * rhs;

  return ret;
}

float Vector3::distance(Vector3 vector) {
  float distance = 0.0f;
  distance += (float) pow(this->x - vector.x, 2);
  distance += (float) pow(this->y - vector.y, 2);
  distance += (float) pow(this->z - vector.z, 2);
  return (float) sqrt(distance);
}

float Vector3::Dot(const Vector3& rhs) const {
  return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

bool Vector3::IsZero() const {
  return this->x + this->y + this->z == 0;
}

float Vector3::distance_to(const Vector3& other) {
  Vector3 delta;
  delta.x = x - other.x;
  delta.y = y - other.y;
  delta.z = z - other.z;
  return float(sqrtf(powf(delta.x, 2.0) + powf(delta.y, 2.0) + powf(delta.z, 2.0)));
}

float Vector3::length() {
  auto sqr = [](float n) {
    return static_cast<float>(n * n);
  };

  return sqrt(sqr(x) + sqr(y) + sqr(z));
}

Vector3 Vector3::Zero() {
  Vector3 ret;
  ret.x = 0;
  ret.y = 0;
  ret.z = 0;
  return ret;
}

Vector2::Vector2() {};

Vector2 Vector2::operator-(const Vector2& rhs) const {
  Vector2 ret;
  ret.x = this->x - rhs.x;
  ret.y = this->y - rhs.y;
  return ret;
}

float Vector2::lenght() {
  return sqrt((x * x) + (y * y));
}

Vector2::Vector2(float x, float y) {
  this->x = x;
  this->y = y;
}


int read_memory(pid_t pid, uintptr_t addr, void* buff, size_t len) {
  memset(buff, 0, len);
  iovec local{}, remote{};
  local.iov_base = buff;
  local.iov_len = len;
  remote.iov_base = (void *) addr;
  remote.iov_len = len;
  return process_vm_readv(pid, &local, 1, &remote, 1, 0);
}

int write_memory(pid_t pid, unsigned long addr, void *buff, size_t len) {
  iovec local{}, remote{};
  local.iov_base = buff;
  local.iov_len = len;
  remote.iov_base = (void *) addr;
  remote.iov_len = len;
  return process_vm_writev(pid, &local, 1, &remote, 1, 0);
}



pid_t pidof(const char *process_name) {
  char cmd[1024]{};
  char buf[100]{};
  snprintf(cmd, 1024, "pidof %s", process_name);
  FILE *fp = popen(cmd, "r");
  fgets(buf, 100, fp);
  pid_t pid = strtoul(buf, NULL, 10);
  pclose(fp);
  return pid;
}

uint64_t get_module_base_by_name_and_length(pid_t pid, const char* path_name, uint64_t length) {
  char filename[100];
  snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    perror("[-] get_module_base");
    exit(-1);
  }
  size_t buf_size = BUFSIZ;
  char* line = (char*) malloc(buf_size);
  while (getline(&line, &buf_size, fp) != -1) {
    uint64_t begin, end;
    char perm[21], offset[21], dev[11], inode[21], path_mem[257];
    sscanf(line, "%lx-%lx %20s %10s %10s %20s %256s", &begin, &end, perm, offset, dev, inode, path_mem);
    if (strstr(path_mem, path_name)) {
      if (end - begin == length) {
        free(line);
        fclose(fp);
        return begin;
      }
    }
  }
  free(line);
  fclose(fp);
  return 0;
}
