#pragma once

template <typename Entity> class IController {
public:
  virtual ~IController() = default;

  virtual void get(int fileDescriptor) const                = 0;
  virtual void getById(int fileDescriptor, size_t id) const = 0;
};