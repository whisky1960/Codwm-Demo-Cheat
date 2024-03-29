#pragma once

#include <cmath>

class Vector3 {
public:
  Vector3() {};

  Vector3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  };

  Vector3 operator-(const Vector3& rhs) const;

  Vector3 operator-=(const Vector3& rhs) const;

  Vector3 operator+(const Vector3& rhs) const;

  Vector3 operator*(const Vector3& rhs) const;

  Vector3 operator/(const Vector3& rhs) const;

  Vector3 operator+=(const Vector3& rhs) const;

  Vector3 operator*=(const Vector3& rhs) const;

  Vector3 operator/=(const Vector3& rhs) const;

  Vector3 operator+(const float& rhs) const;

  Vector3 operator-(const float& rhs) const;

  Vector3 operator/(const float& rhs) const;

  Vector3 operator*(const float& rhs) const;

  float distance(Vector3 vector);

  Vector3 Zero();

  float Dot(const Vector3& rhs) const;

  bool IsZero() const;

  float distance_to(const Vector3& other);

  float length();


public:
  float x;
  float y;
  float z;
};

struct Vector2 {
public:
  float x;
  float y;

  Vector2 operator-(const Vector2& rhs) const;

  float lenght();

  Vector2(float x, float y);

  Vector2();
};

int read_memory(pid_t pid, uintptr_t addr, void* buff, size_t len);
int write_memory(pid_t pid, unsigned long addr, void *buff, size_t len);

template <typename T>
T read(pid_t pid, uintptr_t addr) {
  T res = {};
  read_memory(pid, addr, &res, sizeof(T));
  return res;
}

template <typename T>
int write(pid_t pid, uintptr_t addr, T value) {
  return write_memory(pid, addr, &value, sizeof(T));
}

pid_t pidof(const char *process_name);
uint64_t get_module_base_by_name_and_length(pid_t pid, const char* path_name, uint64_t length);
