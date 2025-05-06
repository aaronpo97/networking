#pragma once
#include <optional>
#include <vector>

/**
 * This is an interface for a repository object that manages a collection of a specific Entity.
 *
 * @tparam Entity The type of entity to be managed by the repository.
 */
template <typename Entity> class IRepository {
public:
  virtual std::optional<Entity> getById(size_t id) const = 0;
  virtual std::vector<Entity>   getAll() const           = 0;
  virtual void                  remove(size_t id)        = 0;
  virtual void                  clear()                  = 0;
};
