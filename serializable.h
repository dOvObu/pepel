#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

struct ser_base
{
	virtual std::string Str() const = 0;
	virtual void WriteTo(std::ofstream& f) = 0;
	virtual void ReadFrom(std::ifstream& f) = 0;
};

template<typename T> std::string __to_string(T v) { return v.Str(); }
template<> std::string __to_string(std::string v) { return '"' + v + '"'; }
template<> std::string __to_string(std::wstring v)
{
	std::string res;
	for (auto& c : v)
	{
		wchar_t n = c >> 8 * sizeof(char);
		res += n;
		res += (c & (n << 8 * sizeof(char)));
	}
	v = L'"' + v + L'"';
	return res;
}
template<> std::string __to_string(long double v) { return std::to_string(v); }
template<> std::string __to_string(float v) { return std::to_string(v); }
template<> std::string __to_string(double v) { return std::to_string(v); }
template<> std::string __to_string(unsigned long long v) { return std::to_string(v); }
template<> std::string __to_string(long long v) { return std::to_string(v); }
template<> std::string __to_string(unsigned long v) { return std::to_string(v); }
template<> std::string __to_string(long v) { return std::to_string(v); }
template<> std::string __to_string(unsigned int v) { return std::to_string(v); }
template<> std::string __to_string(int v) { return std::to_string(v); }

template<typename T> void __write_to(std::ofstream& f, T v) { v.WriteTo(f); }
template<> void __write_to(std::ofstream& f, std::string v)
{
	size_t const sz = v.size();
	f.write((char*)&sz, sizeof(size_t))
	 .write(v.c_str(), sz * sizeof(char));
}
template<> void __write_to(std::ofstream& f, std::wstring v)
{
	std::string&& res = std::move(__to_string(v));
	f.write(res.c_str(), v.size() * sizeof(char));
}
template<> void __write_to(std::ofstream& f, long double v) { f.write((char*)&v, sizeof(long double)); }
template<> void __write_to(std::ofstream& f, float v) { f.write((char*)&v, sizeof(float)); }
template<> void __write_to(std::ofstream& f, double v) { f.write((char*)&v, sizeof(double)); }
template<> void __write_to(std::ofstream& f, unsigned long long v) { f.write((char*)&v, sizeof(unsigned long long)); }
template<> void __write_to(std::ofstream& f, long long v) { f.write((char*)&v, sizeof(long long)); }
template<> void __write_to(std::ofstream& f, long v) { f.write((char*)&v, sizeof(long)); }
template<> void __write_to(std::ofstream& f, unsigned int v) { f.write((char*)&v, sizeof(unsigned int)); }
template<> void __write_to(std::ofstream& f, int v) { f.write((char*)&v, sizeof(int)); }

template<typename T> void __read_from(std::ifstream& f, T& v) { v.ReadFrom(f); }
template<> void __read_from(std::ifstream& f, std::string& v)
{
	size_t sz;
	f.read((char*)&sz, sizeof(size_t));
	char* s = new char[sz];
	f.read(s, sz * sizeof(char));
	v.resize(sz);
	for (size_t i = 0; sz > i; ++i) v[i] = s[i];
	delete[] s;
}
template<> void __read_from(std::ifstream& f, std::wstring& v)
{
	char flag;
	size_t sz, idx{ 0 };
	f.read(&flag, sizeof(char)).read((char*)&sz, sizeof(size_t));
	char* s = new char[sz];
	f.read(s, sz * sizeof(char));
	for (size_t idx = 0; sz > idx; idx += 2)
	{
		wchar_t c = s[idx], c2 = s[idx + 1];
		v += (c << 8 * sizeof(char)) | c2;
	}
	delete[] s;
}
template<> void __read_from(std::ifstream& f, long double& v) { f.read((char*)&v, sizeof(long double)); }
template<> void __read_from(std::ifstream& f, float& v) { f.read((char*)&v, sizeof(float)); }
template<> void __read_from(std::ifstream& f, double& v) { f.read((char*)&v, sizeof(double)); }
template<> void __read_from(std::ifstream& f, unsigned long long& v) { f.read((char*)&v, sizeof(unsigned long long)); }
template<> void __read_from(std::ifstream& f, long long& v) { f.read((char*)&v, sizeof(long long)); }
template<> void __read_from(std::ifstream& f, long& v) { f.read((char*)&v, sizeof(long)); }
template<> void __read_from(std::ifstream& f, unsigned int& v) { f.read((char*)&v, sizeof(unsigned int)); }
template<> void __read_from(std::ifstream& f, int& v) { f.read((char*)&v, sizeof(int)); }



template<typename T>
struct vec : public ser_base {
public:
	vec() = default;
	vec(std::initializer_list<T> const& l) :_v(l) {}
	vec(std::vector<T> const& v) :_v(v) {}
	vec(vec<T> const& v) :_v(v._v) {}

	std::vector<T>* operator->() { return &_v; }
	vec<T>& operator=(std::vector<T> const& v) { _v = v; return *this; }
	vec<T>& operator=(std::initializer_list<T> v) { _v = v; return *this; }
	vec<T>& operator=(vec<T> const& v) { _v = v._v; return *this; }
	T& operator[](size_t const& n) { return _v[n]; }
	vec<T>& operator+=(T const& v) { _v.push_back(v); return *this; }
	vec<T>& operator+=(vec<T> const& r) { for (auto& it : r) _v.push_back(it); return *this; }
	vec<T> operator+(vec<T> const& r) { vec<T> res; res._v = _v; res += r; return res; }
	vec<T> operator+(T const& r) { vec<T> res; res._v = _v; res += r; return res; }

	std::string Str() const override
	{
		std::string res;
		bool first{ true };
		for (auto& it : _v) { if (first) first = false; else res += ','; res += __to_string(it); }
		return res;
	}

	void WriteTo(std::ofstream& f) override
	{
		{
			size_t sz = _v.size();
			f.write((char*)&sz, sizeof(size_t));
		}
		for (auto& it : _v) __write_to(f, it);
	}

	void ReadFrom(std::ifstream& f) override
	{
		{
			size_t sz;
			f.read((char*)&sz, sizeof(size_t));
			_v.resize(sz);
		}
		for (auto& it : _v) __read_from(f, it);
	}
private:
	std::vector<T> _v;
};

template<typename T, typename Base>
struct __serializable : public ser_base
{
private:
	Base* b;
	std::string id;
	T _v;
public:
	__serializable(Base* B, char const* name)
		:b(B), id(name)
	{
		b->members.insert({ name, this });
	}
	__serializable(Base* B, char const* name, T v)
		:b(B), id(name), _v(v)
	{
		b->members.insert({ name, this });
	}

	__serializable<T, Base>& operator=(__serializable<T, Base> const& r)
	{
		_v = r._v;
		return *this;
	}

	__serializable<T, Base>& operator=(T const& r)
	{
		_v = r;
		return *this;
	}

	T* operator->() { return &_v; }
	T& operator()() { return _v; }

	std::string Str() const override { return __to_string<T>(_v); }
	void WriteTo(std::ofstream& f) override { __write_to<T>(f, _v); }
	void ReadFrom(std::ifstream& f) override { __read_from<T>(f, _v); }
};

#define Serializable REDIFINE SERIALIZABLE WITH YOUR CLASS CONTAINER NAME

#define ser_start                                                      \
	struct Serializable : public ser_base                              \
	{                                                                  \
		std::map<std::string, ser_base*> members;

#define ser(type, name, val)                                           \
	__serializable<type, Serializable> name{ this, #name, val };

#define ser_end                                                        \
	std::string Str() const override                                   \
	{                                                                  \
		std::string res{ "{\n" };                                      \
		for (auto& it : members)                                       \
		{                                                              \
			res += it.first + " " + it.second->Str() + "\n";           \
		}                                                              \
		return res + "}";                                              \
	}                                                                  \
                                                                       \
	void WriteTo(std::ofstream& f) override                            \
	{                                                                  \
		{                                                              \
			size_t sz = members.size();                                \
			f.write((char*)&sz, sizeof(size_t));                       \
		}                                                              \
		for (auto& it : members)                                       \
		{                                                              \
			__write_to(f, it.first);                                   \
			it.second->WriteTo(f);                                     \
		}                                                              \
	}                                                                  \
	void ReadFrom(std::ifstream& f) override                           \
	{                                                                  \
		size_t sz;                                                     \
		f.read((char*)&sz, sizeof(size_t));                            \
		for (size_t idx = 0; sz > idx; ++idx)                          \
		{                                                              \
			std::string key;                                           \
            __read_from(f, key);                                       \
			members[key]->ReadFrom(f);                                 \
		}															   \
	}                                                                  \
};

	
#endif // ! SERIALIZABLE_H
