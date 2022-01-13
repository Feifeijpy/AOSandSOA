#pragma once
#include <iostream>
#include <exception>

template <typename T>
class Array3D
{
private:
	unsigned m_pages;
	unsigned m_rows;
	unsigned m_cols;
	T*** m_data_ptr;
	T* m_basic_ptr;

	T*** create3DArray(unsigned pages, unsigned nrows, unsigned ncols, const T& val = T())
	{
		T*** ptr = nullptr;
		T** ptrMem = nullptr;
		T* pool = nullptr;
		try
		{
			ptr = new T * *[pages];  // allocate pointers to pages
			ptrMem = new T * [pages * nrows]; // allocate pointers to pool
			pool = new T[nrows * ncols * pages]{ val };  // allocate pool

		// Assign page pointers to point to the pages memory,
		// and pool pointers to point to each row the data pool
			for (unsigned i = 0; i < pages; ++i, ptrMem += nrows)
			{
				ptr[i] = ptrMem;
				for (unsigned j = 0; j < nrows; ++j, pool += ncols)
					ptr[i][j] = pool;
			}
			//basic = ptr[0][0];
			return ptr;
		}
		catch (std::bad_alloc& ex)
		{
			// rollback the previous allocations
			delete[] ptrMem;
			delete[] ptr;
			throw ex;
		}
	}

public:
	Array3D() : m_pages(0), m_rows(0), m_cols(0), m_data_ptr(nullptr), m_basic_ptr(nullptr) {}
	Array3D(unsigned pages, unsigned rows, unsigned cols, const T& val = T())
	{
		if (pages == 0)
			throw std::invalid_argument("number of pages is 0");
		if (rows == 0)
			throw std::invalid_argument("number of rows is 0");
		if (cols == 0)
			throw std::invalid_argument("number of columns is 0");
		m_data_ptr = create3DArray(pages, rows, cols, val);
		m_basic_ptr = m_data_ptr[0][0];
		m_pages = pages;
		m_rows = rows;
		m_cols = cols;
	}

	~Array3D()
	{
		if (m_data_ptr)
		{
			delete[] m_data_ptr[0][0]; // remove pool
			delete[] m_data_ptr[0];  // remove the pointers
			delete[] m_data_ptr;     // remove the pages
		}
	}

	Array3D(const Array3D& rhs) : m_pages(rhs.m_pages), m_rows(rhs.m_rows), m_cols(rhs.m_cols)
	{
		m_data_ptr = create3DArray(m_pages, m_rows, m_cols);
		std::copy(&rhs.m_data_ptr[0][0], &rhs.m_data_ptr[m_pages - 1][m_rows - 1][m_cols], &m_data_ptr[0][0]);
	}

	Array3D(Array3D&& rhs) noexcept
	{
		m_data_ptr = rhs.m_data_ptr;
		m_pages = rhs.m_pages;
		m_rows = rhs.m_rows;
		m_cols = rhs.m_cols;
		rhs.m_data_ptr = nullptr;
	}

	Array3D& operator=(Array3D&& rhs) noexcept
	{
		if (&rhs != this)
		{
			swap(rhs, *this);
		}
		return *this;
	}

	void swap(Array3D& left, Array3D& right)
	{
		std::swap(left.m_data_ptr, right.m_data_ptr);
		std::swap(left.m_basic_ptr, right.m_basic_ptr);
		std::swap(left.m_pages, right.m_pages);
		std::swap(left.m_rows, right.m_rows);
		std::swap(left.m_cols, right.m_cols);
	}

	Array3D& operator = (const Array3D& rhs)
	{
		if (&rhs != this)
		{
			Array3D temp(rhs);
			swap(*this, temp);
		}
		return *this;
	}

	// typedef T value_type;
	T*** data() const {
		return m_data_ptr;
	}

	T* basic() const {
		return m_basic_ptr;
	}

	unsigned get_pages() const {
		return m_pages;
	}

	unsigned get_rows() const {
		return m_rows;
	}

	unsigned get_cols() const {
		return m_cols;
	}

	T** operator[](unsigned page)
	{
		return m_data_ptr[page];
	}

	const T** operator[](unsigned page) const
	{
		return m_data_ptr[page];
	}

	T& operator()(int i, int j, int k)
	{
		//return m_data_ptr[i][j][k]; // Warning
		return *(m_basic_ptr + i * m_cols * m_rows + j * m_cols + k);
	}

};
