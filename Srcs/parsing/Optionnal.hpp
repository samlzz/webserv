/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Optionnal.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 16:05:45 by sliziard          #+#    #+#             */
/*   Updated: 2025/12/08 16:37:21 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __OPTIONNAL_HPP__
# define __OPTIONNAL_HPP__

# include <cassert>
# include <stdexcept>
# include <string>

// =============================================================================
// Generic Optionnal implementation
// ============================================================================

template<typename T>
class Optionnal {

private:
	bool	_none;
	T		_some;

public:
	// =============================================================================
	// Construction / Destruction
	// ============================================================================

	Optionnal(): _none(true), _some() {}
	Optionnal(const T &val): _none(false), _some(val) {}

	Optionnal(const Optionnal &other)
		: _none(other._none)
	{
		if (!other._none)
			_some = other._some;
	}
	~Optionnal() {}

	// ============================================================================
	// Operators
	// ============================================================================

	Optionnal& operator=(const Optionnal &other)
	{
		if (this != &other)
		{
			_none = other._none;
			_some = other._some;
		}
		return *this;
	}

	Optionnal& operator=(const T &val)
	{
		_none = false;
		_some = val;
		return *this;
	}

	operator bool() const			{ return !_none; }
	const T&	operator*() const
	{
		assert(_none && "Deferencing a None Optionnal"); // ? disabled with -DNDEBUG
		return _some;
	}

	// ============================================================================
	// Methods
	// ============================================================================

	bool	isSome(void) const	{ return !_none; }
	bool	isNone(void) const	{ return _none; }

	const T	*get(void) const
	{
		if (!_none)
			return &_some;
		return 0;
	}

	const T &getOr(const T &def) const
	{
		if (!_none)
			return _some;
		return def;
	}

	const T	&unwrap(const std::string &msg)
	{
		if (_none)
			throw std::runtime_error(msg);
		return _some;
	}

	void	reset(void)
	{
		_none = true;
		_some = T();
	}

	void	set(const T&value)
	{
		*this = value;
	}
};

#endif /* __OPTIONNAL_HPP__ */