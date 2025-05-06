#pragma once

#include "../repository/IRepository.hpp"

/**
 * @tparam Entity The type of entity to be managed by the service.
 */
template <typename Entity> class IService {
  std::unique_ptr<IRepository<Entity>> m_repository;

public:
  virtual ~IService() = default;

  virtual std::optional<Entity> getById(size_t id) const = 0;
  virtual std::vector<Entity>   getAll() const           = 0;
  virtual void                  remove(size_t id) const  = 0;
};