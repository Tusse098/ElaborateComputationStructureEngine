
namespace ecs {

	template<typename... Xs> struct exclude_t {};
	template<typename... Xs> inline constexpr exclude_t<Xs...> exclude{};

}