#pragma once

namespace ecs {

	struct IResource {
		virtual ~IResource() = default;
	};

	template<typename T>
	struct Resource : IResource {
		T value;
		explicit Resource(T v) : value(std::move(v)) {}
	};
}