#pragma once
#include <limits>
#include <vector>
#include <iostream>

template <typename T1>
class CHMProbObjBox
{
public:
	CHMProbObjBox() : m_nCurrentProbObjCount(0)
	{
		std::vector<std::pair<T1, int>>().swap(m_vecProbObjPool);
	}
	~CHMProbObjBox() {};

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Draw a probability object from this box.
	// t1ProbObj:	If this call succeed, the drawn probability object will be put into
	//				t1ProbObj.
	// nRand:		It decides which probability object will be drawn. It should be a positive
	//				random int value(recommended), or -1.
	// Return:		Return true if succeed, false if failed.
	bool Draw(T1& t1ProbObj, const int nRand = -1)
	{
		if (0 >= m_nCurrentProbObjCount) return false;
		if (nRand < 0 && -1 != nRand) return false;

		auto nKeyNum = ((0 > nRand) ? rand() : nRand) % m_nCurrentProbObjCount;
		if (!FindProbObjByRandKey(t1ProbObj, nKeyNum)) return false;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Put in or take out probability objects from this box, so that we can
	//				get a proper probability objects box we wanted.
	// t1ProbObj:	A pointer to the storage of object that you want to put in or take out.
	// pCount:		A pointer to the storage of every object's count.
	// nLen:		The length of storage.
	// Return:		None.
	void Modify(const T1* const t1ProbObj, const int* const pCount, const int nLen = 1)
	{
		if (NULL == t1ProbObj || NULL == pCount || 0 >= nLen) return;

		for (auto i = 0; i < nLen; i++)
		{
			if (0 == pCount[i] || m_scnProbObjBoxCapacity - m_nCurrentProbObjCount < pCount[i]) continue;

			if (!ModifyProbObjPool(t1ProbObj[i], pCount[i])) continue;

			m_nCurrentProbObjCount += pCount[i];
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Put in or take out probability objects from this box, so that we can
	//				get a proper probability objects box we needed.
	// t2ProbObj:	A user-defined container which contains the probability objects we want
	//				to put in or take out and their counts.
	// Return:		None.
	template <typename T2>
	void Modify(const T2& t2ProbObj)
	{
		for (auto it = t2ProbObj.cbegin(); it != t2ProbObj.cend(); it++)
		{
			if (0 == it->second || m_scnProbObjBoxCapacity - m_nCurrentProbObjCount < it->second) continue;

			if (!ModifyProbObjPool(it->first, it->second)) continue;

			m_nCurrentProbObjCount += it->second;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Clear this box to make it empty.
	// Return:		None.
	void Clear()
	{
		m_nCurrentProbObjCount = 0;
		std::vector<std::pair<T1, int>>().swap(m_vecProbObjPool);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get particular or total probability objects counts, depend on pProbObj.
	// pProbObj:	A pointer of the specific probability object, if it == NULL, get the
	//				total count.
	// Return:		The count.
	int GetCount(const T1* const pProbObj = NULL) const
	{
		if (NULL == pProbObj) return m_nCurrentProbObjCount;

		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			if (*pProbObj == it->first) return it->second;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get the probability objects pool.
	// Return:		The probability objects pool.
	const auto& GetPool() const
	{
		return m_vecProbObjPool;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Dump the details of this box.
	// Return:		None.
	void Dump() const
	{
		std::cout << "Current total probability object count " << m_nCurrentProbObjCount << "." << std::endl;
		std::cout << "Probability object box capacity " << m_scnProbObjBoxCapacity << std::endl;

		int nIndex = 0;
		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			nIndex++;
			std::cout << "Probability object index " << nIndex << ", count " << it->second << "." << std::endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Describe:	Get the version of CHMProbObjBox.
	// Return:		A unsigned int stands for the version.
	unsigned int Version() const { return m_scunCHMProbObjBoxVersion; }

private:
	int m_nCurrentProbObjCount;
	std::vector<std::pair<T1, int>> m_vecProbObjPool;
	static const int m_scnProbObjBoxCapacity = INT_MAX;
	static const unsigned int m_scunCHMProbObjBoxVersion = 1;

	bool ModifyProbObjPool(const T1& t1ProbObj, const int nCount)
	{
		for (auto it = m_vecProbObjPool.begin(); it != m_vecProbObjPool.end(); it++)
		{
			if (t1ProbObj == it->first)
			{
				if (0 > it->second + nCount) return false;	// 要删除的奖券不够数，返回错误
				else if (0 == it->second + nCount) m_vecProbObjPool.erase(it);	// 此类奖券被删除完了，移除此项
				else it->second += nCount;	// 加上此类奖券的数量
				return true;
			}
		}
		if (0 > nCount) return false;	// 删除不存在的奖券，返回错误
		else if (0 < nCount)
		{
			try
			{
				m_vecProbObjPool.push_back(std::make_pair(t1ProbObj, nCount));
			}
			catch (const std::exception& e)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), exception: " << e.what() << std::endl;
			}
			catch (...)
			{
				std::cout << __FILE__ << "(" << __LINE__ << "), unknow exception" << std::endl;
			}
		}
		return true;
	}

	bool FindProbObjByRandKey(T1& t1ProbObj, const int nRandKey) const
	{
		auto nBotton = 0, nTop = 0;
		for (auto it = m_vecProbObjPool.cbegin(); it != m_vecProbObjPool.cend(); it++)
		{
			nBotton = nTop;
			nTop += it->second;
			if (nRandKey >= nBotton && nRandKey < nTop)
			{
				t1ProbObj = it->first;
				return true;
			}
		}
		return false;
	}
};
