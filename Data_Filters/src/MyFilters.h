/***************************************************
Copyright (c) 2020 Silmaen

 ****************************************************/

#ifndef _MyFilters_h
#define _MyFilters_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
//#include <WProgram.h>
#include <cstddef>
#endif

/**
 * \brief types of filters
 */
enum struct FilterType {
	MedianFilter, ///< Median value filter
	MeanFilter,   ///< Mean value Filter
	MinFilter,    ///< Min value Filter
	MaxFilter,    ///< Max value Filter
};

template <typename DataType>
class MyFilter
{
public:
	/**
	 * \brief constructor
	 * \param[in] windowSize the width of the window for the filter
	 * \param[in] defaultFilter the filter used for the default filtering
	 */
	MyFilter<DataType>(const int windowSize= 10 , const FilterType& defaultFilter=FilterType::MedianFilter):
		_windowSize{windowSize},_defaultFilter {defaultFilter},defaultFiltered{0}{
		_buffer = new Node[_windowSize];
		_idx = -1;
		first = _buffer;
		last = _buffer;
	}

	/**
	 * \brief add a value in the window
	 * \param[in] item the value to add in the window
	 * \return the filtered value using the default filter
	 *
	 * Once the value is added to the window, the filtered value is then computed and stored
	 */
	DataType addValue(const DataType& item){
		if (_windowSize==0) return DataType();
		if (_windowSize==1) {
			defaultFiltered = item;
			return defaultFiltered;
		}
		if (_idx == -1) {
			// first data insersion
			_idx=0;
			first->data = item;
			first->next = nullptr;
			last = first;
			defaultFiltered = item;
		}else{
			++_idx;
			if (_idx>=_windowSize)_idx=0;
			Node* additem = &_buffer[_idx];
			if (additem == first) {
				first = first->next;
			}else if (additem == last) {
				for(Node* p = first;p!=nullptr;p=p->next){
					if (p->next == last) {
						last = p;
						last->next = nullptr;
					}
				}
			}else{
				for(Node* p = first;p!=nullptr;p=p->next){
				    if (p->next == additem) {
						p->next = additem->next;
						break;
					}
				}
			}
			additem->data = item;
			// insert sortered
			if (additem->data < first->data){ // this is the new min
				additem->next = first;
				first = additem;
			}else if (additem->data >= last->data){ // this is the new max
				last->next = additem;
				last = additem;
				last->next = nullptr;
			}else{
				Node* p = first;
				Node* n = first->next;
				size_t idx = 0;
                for(;n!=nullptr;p=p->next,n=n->next,idx++){
					if (additem->data < n->data){
						p->next = additem;
						additem->next = n;
						break;
					}
				}
			}
			defaultFiltered = getFiltered(_defaultFilter);
			last->next = nullptr;
		}
		return defaultFiltered;
	}

	/**
	 * \brief get the default filtered value
	 * \return the filtered value using the default filter
	 *
	 * The returned value is the precomputed value.
	 */
	DataType getFiltered() const{return defaultFiltered;}
	/**
	 * \brief get the filtered value based on the given filter
	 * \param[in] filter the filter used for the  filtering
	 * \return the filtered value using the default filter
	 *
	 * This value is computed at each call and never stored.
	 */
	DataType getFiltered(const FilterType& filter)const{
		if (_windowSize==0) return DataType();
		if (_windowSize==1) return defaultFiltered;
		DataType valsum = first->data;
		int nbval = 1;
		Node* p = first->next;
        for(;p!=nullptr;p=p->next){
			valsum = valsum + p->data;
			++nbval;
		}
		switch(filter){
		case FilterType::MedianFilter:
			if (nbval==1) return first->data;
			if (nbval%2 != 0) {
				return getNthest(nbval/2)->data;
			}else{
				return (getNthest(nbval/2)->data + getNthest(nbval/2-1)->data)/2;
			}
		case FilterType::MeanFilter:
			return valsum / nbval;
		case FilterType::MinFilter:
			return first->data;
		case FilterType::MaxFilter:
			return last->data;
		}
		return DataType();
	}

private:

    int _windowSize; ///< Size of the windowFilterType _defaultFilter; ///< the default filter to be used
    FilterType _defaultFilter;
    DataType defaultFiltered; ///< storage for the defaultult filtered value
    int _idx;

	struct Node {
		DataType data = DataType();
		Node* next = nullptr;
	};
	/**
	 * \brief the buffer of nodes
	 *
	 * Alloccatred only once at the construction
	 */
	Node* _buffer;
    Node* first;
    Node* last;

	Node* getNthest(const int& n)const{
		if (n<0)return first;
		int i = 0;
		Node* p = first;
		for(; p!=nullptr ; p=p->next, ++i){
			if (i==n) return p;
		}
		return last;
	}


};

#endif
