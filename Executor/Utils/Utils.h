// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once

void MSecToTimeGlobal(int glmsec, int& hour, int& min, int& sec, int& msec);
int TimeToSecGlobal(int hour, int minute, int second=0);
int TimeToMillisecGlobal(int hour, int minute, int second, int milliseond);
std::string* format(const char* fmt_str, ...);
std::string sformat(const char* fmt_str, ...);

template <class T>
class TSearchStockName // Based on hash table
{
	int			_sz;
	int			_deepness;
	std::vector <T*>	_p;

public:
	TSearchStockName(int sz)	// 25000 places for about 7K stocks is with no collusions
	{
		_sz = sz;
		_p.resize(_sz);
		Clear();
	}
	~TSearchStockName()
	{
		for (int ii = 0; ii < (int)_p.size(); ii++)
		{
			T* elem = _p[ii];
			while (elem)
			{
				T* next = elem->GetNext();
				delete elem;
				elem = next;
			}
		}
	}
	void Clear()
	{
		for (int ii = 0; ii < _sz; ii++)
			_p[ii] = NULL;
		_deepness = 0;
	}

	bool AddToListFast(T& elem)
	{
		// Return 'true' if added; 'false' if name already existed in the list
		//
		int idx = CalcIndex(elem.Name());
		if (idx > -1 && idx < _sz)
		{
			if (_p[idx] == NULL)
			{
				_p[idx] = new T(elem);
				return true;
			}

			T* in = _p[idx];
			int deepness = 1;
			do
			{
				if (in->Compare(elem.Name()) == 0)
					return false;		// Same name already in the list
				if (in->GetNext() == NULL)
				{
					in->SetNext(new T(elem));
					if (deepness > _deepness)
						_deepness = deepness;
					return true;
				}
				else
					deepness++;
				in = in->GetNext();
			} while (in);

			return false; // Shouldn't happen
		}
		return false;	// Error
	}

	T* FindFast(const char* s)
	{
		int idx = CalcIndex(s);
		if (idx > -1)
		{
			T* out = _p[idx];
			while (out)
			{
				if (out->Compare(s) == 0)
					return out;
				out = out->GetNext();
			}
			if (out == NULL)
				return NULL;
		}
		return NULL;
	}

protected:
	int CalcIndex(const char* s)
	{
		if (s)
		{
			unsigned int idx = 0;
			do
			{
				idx = idx * 31 + *s;
			} while (*(++s));

			idx = idx % _sz;
			return (int)idx; // Ok to convert to int as long as _sz in below max possible int value
		}
		return -1;
	}
};
