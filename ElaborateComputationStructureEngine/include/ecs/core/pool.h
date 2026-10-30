// include/ecs/core/pool.h
#pragma once
#include <vector>
#include <ecs/core/entity.h>"

namespace ecs {

struct IPool {
	virtual ~IPool() = default;
	virtual bool has(Entity e) const = 0;
	virtual void remove(Entity e) = 0;
};

template<typename T>
class Pool : public IPool {
public:
	void add(Entity e, T value) {
		entities.push_back(e);
		dense.push_back(value);

		if (e >= sparse.size()) sparse.resize(e + 1, NONE);
		sparse.at(e) = entities.size() - 1;
	}

	void remove(Entity e) {
		if (!has(e)) return;
		uint32_t idx = sparse[e];
		uint32_t last = static_cast<uint32_t>(size() - 1);

		dense[idx] = dense[last];
		entities[idx] = entities[last];
		sparse[entities[idx]] = idx;

		dense.pop_back();
		entities.pop_back();
		sparse[e] = NONE;
	}

	T& get(Entity e) {
		return dense[sparse[e]];
	}

	bool has(Entity e) const {
		if (e >= sparse.size()) return false;
		if (sparse[e] != NONE) return true;
		return false;
	}

	size_t size() const { return dense.size(); }

	std::vector<Entity>&	owners() { return entities; }
	std::vector<T>&			data() { return dense; }

private:
	static constexpr uint32_t NONE = UINT32_MAX;
	std::vector<Entity>		entities;	// The entities related to the data
	std::vector<T>			dense;		// Where the data lives
	std::vector<uint32_t>	sparse;
};

}