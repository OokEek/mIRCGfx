#pragma once

namespace mIRCGfx {
	template<class _Ty>
	struct is_pod
		: std::integral_constant<bool, std::is_standard_layout_v<_Ty>&& std::is_trivial_v<_Ty> >
	{
	};

	template <typename T>
	constexpr bool is_pod_v = is_pod<T>::value;

	template <typename Input>
	concept IsPODArray = std::is_array_v<Input> && mIRCGfx::is_pod_v<Input>;

	struct MapFile {
		HANDLE m_map{ nullptr };
		TCHAR *m_data{ nullptr };
		int		m_cnt{};

		explicit operator bool() const noexcept { return (m_map && m_data); }

		MapFile() = default;

		bool operator==(const MapFile& other) const = default;

		MapFile(const HANDLE& m_map, TCHAR* m_data, int m_cnt) noexcept
			: m_map(m_map), m_data(m_data), m_cnt(m_cnt)
		{
		}
	};

	void load(LOADINFO *const lInfo) noexcept;
	void unload() noexcept;

	bool mIRC_SendMessage(const MapFile &mf, const uint32_t uMsg) noexcept;

	template <typename Input>
	bool exec(const MapFile &mf, const Input &data) noexcept
	{
		if constexpr(IsPODArray<Input>)
			_ts_strcpyn(mf.m_data, std::addressof(data[0]), MIRC_BUFFER_SIZE_CCH);
		else if constexpr(std::is_pointer_v<Input>)
			_ts_strcpyn(mf.m_data, data, MIRC_BUFFER_SIZE_CCH);
		else
			_ts_strcpyn(mf.m_data, data.c_str(), MIRC_BUFFER_SIZE_CCH);

		{
			// SendMessage(mHwnd, WM_MCOMMAND, MAKEWPARAM(cMethod, cEventId), cIndex)
			if (mIRC_SendMessage(mf, WM_MCOMMAND))
				return _ts_isEmpty(mf.m_data);
		}
		return false;
	}

	template <typename Input, typename Value, typename... Arguments>
	bool exec(const MapFile &mf, const Input &fmt, const Value &val, Arguments&&... args)
	{
		std::wstring line;
		return exec(mf, _ts_sprintf(line, fmt, val, args...));
	}

	template <typename Output, typename Input>
	GSL_SUPPRESS(bounds.1) std::optional<Output> o_eval(const MapFile &mf, const Input &data) noexcept
	{
		if constexpr(IsPODArray<Input>)
			//_ts_strcpyn(mf.m_data, &data[0], std::min<size_t>(std::extent_v<decltype(data)>, MIRC_BUFFER_SIZE_CCH));
			_ts_strcpyn(mf.m_data, std::addressof(data[0]), MIRC_BUFFER_SIZE_CCH);
		else if constexpr(std::is_pointer_v<Input>)
			_ts_strcpyn(mf.m_data, data, MIRC_BUFFER_SIZE_CCH);
		else
			_ts_strcpyn(mf.m_data, data.c_str(), MIRC_BUFFER_SIZE_CCH);

		{
			if (mIRC_SendMessage(mf, WM_MEVALUATE))
				return { mf.m_data };
		}
		mf.m_data[0] = 0;
		return {};
	}
	template <typename Output, typename Input, typename Value, typename... Arguments>
	std::optional<Output> o_eval(const MapFile &mf, const Input &fmt, const Value &val, Arguments&&... args)
	{
		std::wstring line;
		return o_eval<Output>(mf, _ts_sprintf(line, fmt, val, args...));
	}
	template <typename Output, typename Input, typename Value>
	std::optional<Output> o_eval(const MapFile &mf, const Input &fmt, const Value &val)
	{
		std::wstring line;
		return o_eval<Output>(mf, _ts_sprintf(line, fmt, val));
	}

	template <typename... Arguments>
	bool exec_format(const MapFile& mf, const std::wformat_string<Arguments...> fmt, Arguments&&... args)
	{
		return exec(mf, std::vformat(fmt.get(), std::make_wformat_args(args...)));
	}
	template <typename Output, typename... Arguments>
	std::optional<Output> o_eval_format(const MapFile& mf, const std::wformat_string<Arguments...> fmt, Arguments&&... args)
	{
		return o_eval<Output>(mf, std::vformat(fmt.get(), std::make_wformat_args(args...)));
	}

	template <typename T>
	T StringToPointer(const TCHAR* str) noexcept
	{
		if (!str)
			return T{};

		TCHAR* ptr{};

		// only allows decimal numbers
		//return reinterpret_cast<T>(_ts_strtoumax(t, &ptr, 10));

		//allows hex & decimal, hex numbers must start 0x or 0X or # for web style
		if (_ts_strnicmp(str, TEXT("0x"), 2) == 0)	// base 16 hex number 0x00000000 - 0xFFFFFFFF for 32bit or 0x0000000000000000 - 0xFFFFFFFFFFFFFFFF for 64bit
			return reinterpret_cast<T>(_ts_strtoumax(str, &ptr, 16));
		if (str[0] == '#')	// base 16 hex number #00000000 - #FFFFFFFF for 32bit or #0000000000000000 - #FFFFFFFFFFFFFFFF for 64bit
			return reinterpret_cast<T>(_ts_strtoumax(&str[1], &ptr, 16));

		// decimal number
		return reinterpret_cast<T>(_ts_strtoumax(str, &ptr, 10));
	}
}