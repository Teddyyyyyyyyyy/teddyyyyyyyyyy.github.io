#include<iostream>
#include<cstring>
#include<string>
#include<cstdlib> // malloc free
#include<cassert>

using namespace std;
class MyString
{
public:
	static const int BufSize = 16;
	struct StrNode
	{
		int ref;//指向柔性数组的对象个数
		int len;//数组内元素个数
		int size;//数组总大小，不包含最后标记位\0
		char data[];//柔性数组中括号内给0或者不给都可以，只占位
		//sizeof(StrNode)为12不是16
	};
private:
	StrNode* pstr;
	//申请mem
	static StrNode* BuyNode(size_t sz)
	{
		StrNode* s = (StrNode*)malloc(sizeof(StrNode) + sz);
		if (s == nullptr)
		{
			exit(1);
		}
		return s;
	}
	//释放结点
	static void FreeNode(StrNode* snp)
	{
		free(snp);
	}

	void _Init(const char* cp)
	{
		int len = strlen(cp);
		size_t size = (len * 2) < BufSize ? BufSize : (len * 2);
		pstr = BuyNode(size);
		pstr->ref = 1;
		pstr->len = len;
		pstr->size = size - 1;
		strcpy_s(pstr->data, pstr->size + 1, cp);
	}
	void _IncMem(int newsz = 0) //pstr != nullptr  pstr->ref = 1;
	{
		int len = pstr->len;
		size_t size = (pstr->size + 1) * 2;
		if (newsz != 0)
		{
			size = (pstr->len + newsz) * 2;
		}

		StrNode* newstr = BuyNode(size);
		newstr->ref = 1;
		newstr->len = len;
		newstr->size = size - 1;
		strcpy_s(newstr->data, newstr->size + 1, pstr->data);
		FreeNode(pstr);
		pstr = newstr;
	}
	StrNode* CopyPstr(StrNode* p) //p != nullptr // p->ref > 1;
	{
		int len = p->len;
		size_t size = (p->size + 1);
		StrNode* newstr = BuyNode(size);
		memmove(newstr, p, sizeof(StrNode) + len + 1);
		p->ref -= 1;
		newstr->ref = 1;
		return newstr;
	}

public:
	MyString(const char* cp = nullptr) :pstr(nullptr)
	{
		if (nullptr != cp)
		{
			_Init(cp);
		}
	}
	MyString(const MyString& s) :pstr(s.pstr)
	{
		if (nullptr != pstr) // if(pstr != nullptr)
		{
			pstr->ref += 1;
		}
	}
	MyString& operator=(const MyString& s)
	{
		if (this == &s || this->pstr == s.pstr) return *this;
		if (nullptr != this->pstr && --this->pstr->ref == 0)
		{
			FreeNode(this->pstr);
			this->pstr = nullptr;
		}
		this->pstr = s.pstr;
		if (nullptr != this->pstr)
		{
			this->pstr->ref += 1;
		}
		return *this;
	}
	// this == &s; this->pstr = nullptr  s.pstr == nullptr;
	// this->pstr != nullptr 


	~MyString()
	{
		if (nullptr != pstr && --pstr->ref == 0)
		{
			FreeNode(pstr);
		}
		pstr = nullptr;
	}

	MyString(MyString&& s) :pstr(s.pstr)
	{
		s.pstr = nullptr;
	} // MyString(s);

	//MyString & operator=(MyString * const this, MyString &&s)
	// s1 = std::move(s2);
	// s1.operator=(std::move(s2));
	//    operator=(&s1,std::move(s2));

	MyString& operator=(MyString&& s)
	{
		if (this == &s ||
			(nullptr == this->pstr && nullptr == s.pstr))
			return *this;
		//	this->pstr == s.pstr;
		// s1 = s2;
		if (this->pstr == s.pstr)
		{
			s.pstr->ref -= 1;
			s.pstr = nullptr;
			return *this;
		}
		if (nullptr != this->pstr && --this->pstr->ref == 0)
		{
			FreeNode(this->pstr);
			this->pstr = nullptr;
		}
		this->pstr = s.pstr;
		s.pstr = nullptr;
		return *this;
	}
public:
	char& at(size_t pos)
	{
		assert(pos >= 0 && pos < pstr->len);
		return pstr->data[pos];
	}
	const char& at(size_t pos) const
	{
		assert(nullptr != pstr && pos >= 0 && pos < pstr->len);
		return pstr->data[pos];
	}

	char& operator[](size_t pos)
	{
		assert(nullptr != pstr && pos >= 0 && pos < pstr->len);
		if (pstr->ref > 1)
		{
			pstr = CopyPstr(pstr);
		}
		return pstr->data[pos];
	}
	const char& operator[](size_t pos) const
	{
		assert(nullptr != pstr && pos >= 0 && pos < pstr->len);
		return pstr->data[pos];
	}

	char& front()
	{
		assert(nullptr != pstr);
		return pstr->data[0];
	}
	const char& front()const
	{
		assert(nullptr != pstr);
		return pstr->data[0];
	}

	char& back()
	{
		assert(nullptr != pstr && pstr->len > 0);
		return pstr->data[pstr->len - 1];
	}
	const char& back()const
	{
		assert(nullptr != pstr && pstr->len > 0);
		return pstr->data[pstr->len - 1];
	}

	char* data()
	{
		assert(pstr != nullptr);
		return pstr->data;
	}
	const char* data() const
	{
		assert(pstr != nullptr);
		return pstr->data;
	}

	const char* c_str() const
	{
		assert(pstr != nullptr);
		return pstr->data;
	}

	bool empty() const
	{
		assert(pstr != nullptr);
		return pstr->len == 0;
	}
	size_t size() const
	{
		assert(pstr != nullptr);
		return pstr->len;
	}
	size_t length() const
	{
		assert(pstr != nullptr);
		return pstr->len;
	}
	size_t capacity() const
	{
		assert(pstr != nullptr);
		return pstr->size;
	}
	void resever(size_t new_cap)
	{
		if (nullptr == pstr)
		{
			pstr = BuyNode(new_cap);
			pstr->ref = 1;
			pstr->len = 0;
			pstr->size = new_cap - 1;
			pstr->data[0] = '\0';
		}
		else if (new_cap > (pstr->size * 1.5))
		{
			StrNode* newstr = BuyNode(new_cap);
			newstr->ref = 1;
			newstr->len = pstr->len;
			newstr->size = new_cap - 1;
			strcpy_s(newstr->data, newstr->size + 1, pstr->data);
			if (--pstr->ref == 0)
			{
				FreeNode(pstr);
			}
			pstr = newstr;
		}
	}
	MyString& insert(size_t index, const char ch)
	{
		if (nullptr == pstr && index == 0)
		{
			char str[2] = { ch };// ch,\0;
			_Init(str);
		}
		else
		{
			assert(nullptr != pstr && index >= 0 && index <= pstr->len);
			if (pstr->ref > 1)
			{
				pstr = CopyPstr(pstr);
			}
			if (pstr->len + 1 > pstr->size)
			{
				_IncMem();
			}
			memmove(pstr->data + index + 1, pstr->data + index, pstr->len - index + 1);
			pstr->data[index] = ch;
			pstr->len += 1;
		}
		return *this;
	}
	MyString& insert(size_t index, const char* p)
	{
		if (nullptr == p) return *this;

		if (nullptr == pstr && index == 0)
		{
			_Init(p);
		}
		else
		{
			assert(nullptr != pstr && index >= 0 && index <= pstr->len);
			if (pstr->ref > 1)
			{
				pstr = CopyPstr(pstr);
			}
			int len = strlen(p);
			if (pstr->len + len > pstr->size)
			{
				_IncMem(len);
				//_IncMem(len, p);
			}
			memmove(pstr->data + index + len, pstr->data + index, pstr->len - index + 1);
			memmove(pstr->data + index, p, len);
			pstr->len += len;
		}
		return *this;
	}
	MyString& insert(size_t index, const MyString& s)
	{
		if (nullptr == s.pstr)
		{
			return *this;
		}
		else
		{
			return insert(index, s.pstr->data);
		}
	}

	MyString& operator+=(const char ch)
	{
		if (pstr != nullptr)
		{
			insert(pstr->len, ch);
		}
		else
		{
			insert(0, ch);
		}
		return *this;
	} //s1+="yhping";
	MyString& operator+=(const char* cp)
	{
		assert(cp != nullptr);
		if (pstr != nullptr)
		{
			insert(pstr->len, cp);
		}
		else
		{
			insert(0, cp);
		}
		return *this;
	}
	MyString& operator+=(const MyString& s)
	{
		if (pstr != nullptr)
		{
			insert(pstr->len, s);
		}
		else
		{
			insert(0, s);
		}
	}
	void push_back(const char ch)
	{
		*this += ch;
	}
	void push_back(const char* cp)
	{
		*this += cp;
	}
	void push_back(const MyString& s)
	{
		*this += s;
	}
	MyString& erase(size_t index)
	{
		if (nullptr == pstr) return *this;
		assert(index < pstr->len);
		if (pstr->ref > 1)
		{
			pstr = CopyPstr(pstr);
		}
		memmove(pstr->data + index, pstr->data + index + 1, pstr->len - index);
		pstr->len -= 1;
		return *this;
	}

	MyString& erase(size_t index, size_t count);


	void clear()
	{
		if (pstr != nullptr && pstr->ref > 1)
		{
			pstr = CopyPstr(pstr);
		}
		if (pstr != nullptr)
		{
			pstr->len = 0;
		}
	}
	ostream& operator<<(ostream& out) const
	{
		if (pstr != nullptr)
		{
			out << "Ref: " << pstr->ref << " Len: " << pstr->len << " Cap: " << pstr->size << endl;
			out << pstr->data;
		}
		return out;
	}
	istream& operator>>(istream& in)
	{
		char ch;
		clear();
		while (in.get(ch), ch != '\n')
		{
			*this += ch;
		}
		return in;
	}
};
ostream& operator<<(ostream& out, const MyString& s)
{
	s << out;
	return out;
}
istream& operator>>(istream& in, MyString& s)
{
	s >> in;
	return in;
}
int main()
{
	MyString s1("newdata");
	cout << s1 << endl;
	cin >> s1;
	cout << s1 << endl;
	return 0;
}
