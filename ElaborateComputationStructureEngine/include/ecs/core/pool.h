// include/ecs/core/pool.h
#pragma once

#include <vector>
#include <ecs/core/entity.h>
#include <cassert>


namespace ecs {

	struct IPool {
		virtual ~IPool() = default;
		virtual bool has(Entity e) const = 0;
		virtual void remove(Entity e) = 0;
		virtual size_t size() const = 0;
		virtual const std::vector<Entity>& owners() const = 0;
	};

	template<typename T>
	class Pool : public IPool {
	public:

		void add(Entity e, T value) {
			assert(!has(e) && "component added twice to the same entity");
			uint32_t idx = index(e);
			entities.push_back(e);
			dense.push_back(value);

			if (idx >= sparse.size()) sparse.resize(idx + 1, NONE);
			sparse.at(idx) = entities.size() - 1;
		}

		void remove (Entity e) override {
			if (!has(e)) return;
			uint32_t idx = sparse[index(e)];
			uint32_t last = static_cast<uint32_t>(size() - 1);

			dense[idx] = dense[last];
			entities[idx] = entities[last];
			sparse[index(entities[idx])] = idx;

			dense.pop_back();
			entities.pop_back();
			sparse[index(e)] = NONE;
		}

		T& get(Entity e) {
			return dense[sparse[index(e)]];
		}

		 bool has(Entity e) const override {
			if (index(e) >= sparse.size()) return false;
			if (sparse[index(e)] != NONE) return true;
			return false;
		}

		size_t					size()	const override { return dense.size(); }
		const std::vector<Entity>&	owners() const override { return entities; }
		std::vector<T>&			data() { return dense; }

	private:
		static constexpr uint32_t NONE = UINT32_MAX;
		std::vector<Entity>		entities;	// The entities related to the data
		std::vector<T>			dense;		// Where the data lives
		std::vector<uint32_t>	sparse;		// Entity -> dense position
	};

}