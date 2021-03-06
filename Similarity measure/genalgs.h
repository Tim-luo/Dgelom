/***************************************************************************
This file is part of Matrice, an effcient and elegant C++ library for SC.
Copyright(C) 2018, Zhilong (Dgelom) Su, all rights reserved.

This program is free software : you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/
#pragma once
#include<algorithm>
#include "_macros.h"
#include "../arch/ixpacket.h"

MATRICE_NAMESPACE_BEGIN_
template<class _Fn, class _InIt, class _OutIt> inline
void transform(_Fn _Func, const _InIt _First, const _InIt _Last, size_t _Stride, _OutIt _Dest, size_t _Invpos = 1)
{
	using namespace std;
	auto _UFirst = _Unchecked(_First + _Stride - _Invpos);
	_DEBUG_RANGE(_UFirst, _Last);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = _Unchecked_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
	for (; (_UFirst + _Invpos) != _ULast; _UFirst += _Stride, (void)++_UDest)
	{
		*_UDest = _Func(*_UFirst);
	}
	*(_UDest) = _Func(*_UFirst);
}
template<class _Fn, class _InIt, class _OutIt> inline
void transform(_Fn _Func, const _InIt _First, const _InIt _Last, _OutIt _Dest, size_t _Stride)
{
	using namespace std;
	_DEBUG_RANGE(_First, _Last);
	auto _UFirst = _Unchecked(_First);
	const auto _ULast = _Unchecked(_Last);
	auto _UDest = _Unchecked_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
	for (; _UFirst != _ULast; ++_UFirst, (void)(_UDest+=_Stride))
	{
		*_UDest = _Func(*_UFirst);
	}
}
template<typename _Ty, typename _InIt = _Ty*>
MATRICE_GLOBAL_INL const _Ty reduce(_InIt _First, _InIt _Last)
{
	static_cast<void>(_First == _Last);
	_Ty _Ret = 0;
#ifdef __AVX__
	using Packed_t = simd::Packet_<_Ty, 4>;
	decltype(auto) _Size = std::_Idl_distance<_InIt>(_First, _Last);
	decltype(auto) _Step = Packed_t::size << 1;
	decltype(auto) _N = _Size / _Step;
	for (auto i = 0, j = 0; i < _N; j = (++i)*_Step)
		_Ret += (Packed_t(_First + j) + Packed_t(_First + j +Packed_t::size)).reduce();
	for (_First += _N * _Step; _First != _Last; ++_First) _Ret += *_First;
#else
	for (; _First != _Last; ++_First) _Ret += *_First;
#endif
	return (_Ret);
}
template<typename _Ty, typename _Op,  typename _InIt = _Ty*>
MATRICE_GLOBAL_INL _Ty reduce(_InIt _First, _InIt _Last, _Op _op)
{
	static_cast<void>(_First == _Last);
	_Ty _Ret = 0;
	for (; _First != _Last; ++_First) _Ret += _op(_First);
	return (_Ret);
}
template<typename _InIt, typename _Op, typename = typename std::enable_if_t<std::is_pointer<_InIt>::value>::type>
MATRICE_GLOBAL_INL auto reduce(_InIt _First, _InIt _Last, _Op _op)
{
	static_cast<void>(_First == _Last);
	typename remove_reference<decltype(_First[0])>::type _Ret = 0;
	for (; _First != _Last; ++_First) _Ret += _op(_First);
	return (_Ret);
}
_MATRICE_NAMESPACE_END
