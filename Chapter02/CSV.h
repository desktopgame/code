#pragma once
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>

template<typename T>
class CSVRow {
public:
	explicit CSVRow();

	void AddElement(const T& elem);
	void RemoveElement(int index);
	const T& GetElementAt(int index) const;
	size_t Count() const;
private:
	std::vector<T> elements;
};

template<typename T>
class CSV {
public:
	explicit CSV();
	void AddRow(const CSVRow<T>& row);
	void RemoveRow(int index);
	const CSVRow<T>& GetRowAt(int index) const;
	size_t Count() const;
private:
	std::vector<CSVRow<T> > rows;
};

template<typename T>
CSV<T> readCSV(const std::string& filename, std::function<T(const std::string&)> func);

template<typename T>
CSV<T> readCSV(const char* filename, std::function<T(const std::string&)> func);


// CSVRow
template<typename T>
inline CSVRow<T>::CSVRow() : elements() {
}

template<typename T>
inline void CSVRow<T>::AddElement(const T & elem) {
	elements.emplace_back(elem);
}

template<typename T>
inline void CSVRow<T>::RemoveElement(int index) {
	elements.erase(elements.begin() + index);
}

template<typename T>
inline const T & CSVRow<T>::GetElementAt(int index) const {
	return elements.at(index);
}

template<typename T>
inline size_t CSVRow<T>::Count() const {
	return elements.size();
}

// CSV
template<typename T>
inline CSV<T>::CSV() : rows() {
}

template<typename T>
inline void CSV<T>::AddRow(const CSVRow<T>& row) {
	rows.emplace_back(row);
}

template<typename T>
inline void CSV<T>::RemoveRow(int index) {
	rows.erase(rows.begin() + index);
}

template<typename T>
inline const CSVRow<T>& CSV<T>::GetRowAt(int index) const {
	return rows.at(index);
}

template<typename T>
inline size_t CSV<T>::Count() const {
	return rows.size();
}

// Function
template<typename T>
inline CSV<T> readCSV(const std::string & filename, std::function<T(const std::string&)> func) {
	std::ifstream ifs(filename);
	if (ifs.fail()) {
		throw std::logic_error(filename + " is not found");
	}
	CSV<T> ret;
	std::string line;
	std::stringstream buf;
	int width = 0;
	while (getline(ifs, line)) {
		buf.str("");
		buf.clear(std::stringstream::goodbit);
		CSVRow<T> row;
		for (int i = 0; i < line.size(); i++) {
			char c = line.at(i);
			if (c == ',') {
				T data = func(buf.str());
				row.AddElement(data);
				buf.str("");
				buf.clear(std::stringstream::goodbit);
			} else {
				buf << c;
			}
		}
		std::string tail = buf.str();
		if (tail.size() > 0) {
			row.AddElement(func(buf.str()));
		}
		if (width == 0) {
			width = row.Count();
		} else if (width != row.Count()) {
			throw std::logic_error("invalid row: " + line);
		}
		ret.AddRow(row);
	}
	return ret;
}

template<typename T>
inline CSV<T> readCSV(const char * filename, std::function<T(const std::string&)> func) {
	return readCSV<T>(std::string(filename), func);
}
