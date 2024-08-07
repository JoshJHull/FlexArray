#include "FlexArray.h"

FlexArray::FlexArray() {
    size_ = 0;
    capacity_ = INITIALCAP;
    arr_ = new int[INITIALCAP];
    if (capacity_ % 2 == 0) {
        headroom_ = capacity_ / 2;
        tailroom_ = capacity_ / 2;
    } else {
        headroom_ = capacity_ / 2;
        tailroom_ = (capacity_ / 2) + 1;
    }
}

FlexArray::FlexArray(const int* arr, int size) {
    size_ = size;
    capacity_ = LO_THRESHOLD * size;
    headroom_ = size;
    tailroom_ = size;
    arr_ = new int[capacity_];
    for (int i = 0; i < size; i++) {
        arr_[headroom_ + i] = arr[i];
    }
}

FlexArray::~FlexArray() {
	delete [] arr_;
}

FlexArray::FlexArray(const FlexArray& other) {
	size_ = other.size_;
    capacity_ = other.capacity_;
    headroom_ = other.headroom_;
    tailroom_ = other.tailroom_;
    arr_ = new int[capacity_];
    for (int i = 0; i < size_; i++) {
        arr_[headroom_ + i] = other.get(i);
    }
}

FlexArray& FlexArray::operator=(const FlexArray& other) {
	if (this != &other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        headroom_ = other.headroom_;
        tailroom_ = other.tailroom_;
        delete [] arr_;
        arr_ = new int[capacity_];
        for (int i = 0; i < size_; i++)
            arr_[headroom_ + i] = other.get(i);
    }

	return *this;
}

int FlexArray::getSize() const {
	return size_;
}

int FlexArray::getCapacity() const {
	return capacity_;
}

string FlexArray::print() const {
    if (size_ == 0)
        return "[]";
	string str = "[";
    for (int i = 0; i < (size_ - 1); i++)
        str.append(std::to_string(arr_[headroom_ + i]) + ", ");
    str.append(std::to_string(arr_[headroom_ + (size_ - 1)]));
    str.append("]");

	return str;
}

string FlexArray::printAll() const {
	string str = "[";
    for (int i = 0; i < headroom_; i++)
        str.append("X, ");
    for (int i = 0; i < size_; i++) {
        if (i == (size_ - 1) && tailroom_ == 0) {
            str.append(std::to_string(arr_[headroom_ + i]));
            break;
        }
        str.append(std::to_string(arr_[headroom_ + i]) + ", ");
    }
    for (int i = 0; i < tailroom_; i++) {
        if (i == (tailroom_ - 1)) {
            str.append("X");
            break;
        }
        str.append("X, ");
    }

    str.append("]");

	return str;
}

int FlexArray::get(int i) const {
	return arr_[headroom_ + i];
}

bool FlexArray::set(int i, int v) {
	if (i < 0 || i >= size_)
        return false;

    arr_[headroom_ + i] = v;
	return true;
}

void FlexArray::push_back(int v) {
    if (tailroom_ == 0) {
        expandCapacity();
    }
	if (size_ == 0) {
        if (capacity_ % 2 == 0) { //if even empty array place left of centre
            arr_[headroom_ - 1] = v;
            headroom_ -= 1;
        }
        else {
            arr_[headroom_] = v;
            tailroom_ -= 1;
        }
        size_ += 1;
        return;
    }
    arr_[headroom_ + size_] = v;
    size_ += 1;
    tailroom_ -= 1;
}

bool FlexArray::pop_back() {
    if (size_ == 0)
        return false;
	tailroom_ += 1;
    size_ -= 1;
    emptyRecenter();
    shrinkCapacity();
    return true;
}

void FlexArray::push_front(int v) {
    if (headroom_ == 0) {
        expandCapacity();
    }
    if (size_ == 0) {
        if (capacity_ % 2 == 0) { //if even empty array place left of centre
            arr_[headroom_ - 1] = v;
            headroom_ -= 1;
        }
        else {
            arr_[headroom_] = v;
            tailroom_ -= 1;
        }
        size_ += 1;
        return;
    }
    arr_[headroom_ - 1] = v;
    size_ += 1;
    headroom_ -= 1;
}

bool FlexArray::pop_front() {
    if (size_ == 0)
        return false;
	headroom_ += 1;
    size_ -= 1;
    emptyRecenter();
    shrinkCapacity();
    return true;
}

bool FlexArray::insert(int i, int v) {
	if (i < 0 || i > size_) {
        return false;
    }
    if (headroom_ == 0 && tailroom_ == 0)
        expandCapacity();
    if (size_ == 0 && i == 0) {
        push_front(v);
        return true;
    }
    if (i == 0) { //want to add to beginning of array
        if (headroom_ == 0) { //no room in head move all values right
            for (int j = i; j < size_; j++) {
                arr_[headroom_ + (j + 1)] = arr_[headroom_ + j];
            }
            arr_[headroom_ + i] = v;
            tailroom_ -= 1;
            size_ += 1;
            return true;
        }
        arr_[headroom_ - 1] = v;
        size_ += 1;
        headroom_ -= 1;
        return true;
    }
    if (i <= (size_ - 1) / 2) { //if left side is smaller want to shift left towards head
        //if no headroom shift right towards tail
        if (headroom_ == 0) {
            int it = size_;
            for (int j = i; j < size_; j++) {
                arr_[headroom_ + it] = arr_[headroom_ + it - 1];
                it -= 1;
            }
            arr_[headroom_ + i] = v;
            tailroom_ -= 1;
            size_ += 1;
            return true;
        }
        //shift left towards head
        int it {0};
        for (int j = i; j > 0; j--) {
            arr_[(headroom_ - 1) + it] = arr_[headroom_ + it];
            it += 1;
        }
        headroom_ -= 1;
        arr_[headroom_ + i] = v;
        size_ += 1;
        return true;
    }
    //right side is smaller so want to shift right towards tail
    if (tailroom_ == 0) {
        //if no tailroom shift left towards head
        int it {0};
        for (int j = i; j > 0; j--) {
            arr_[(headroom_ - 1) + it] = arr_[headroom_ + it];
            it += 1;
        }
        headroom_ -= 1;
        arr_[headroom_ + i] = v;
        size_ += 1;
        return true;
    }
    //shift right towards tail
    int it = size_;
    for (int j = i; j < size_; j++) {
        arr_[headroom_ + it] = arr_[headroom_ + it - 1];
        it -= 1;
    }
    arr_[headroom_ + i] = v;
    tailroom_ -= 1;
    size_ += 1;
    return true;
}

bool FlexArray::erase(int i) {
    if (size_ == 0 || i >= size_ || i < 0)
        return false;
    if (i == 0) {
        headroom_ += 1;
        size_ -= 1;
        emptyRecenter();
        shrinkCapacity();
        return true;
    }
    if (i == (size_ - 1) / 2 && (size_ % 2) != 0) { //if centre element shift right side towards head
        for (int j = i; j < size_; j++) {
            arr_[headroom_ + j] = arr_[headroom_ + (j + 1)];
        }
        size_ -= 1;
        tailroom_ += 1;
        emptyRecenter();
        shrinkCapacity();
        return true;
    }
    if (i <= (size_ - 1) / 2) {
        //shift left side towards tail
        for (int j = i; j >= 0; j--) {
            arr_[headroom_ + j] = arr_[headroom_ + (j - 1)];
        }
        headroom_ += 1;
        size_ -= 1;
        emptyRecenter();
        shrinkCapacity();
        return true;
    }
    //shift right side towards head
    for (int j = i; j < size_; j++) {
        arr_[headroom_ + j] = arr_[headroom_ + (j + 1)];
    }
    size_ -= 1;
    tailroom_ += 1;
    emptyRecenter();
    shrinkCapacity();
    return true;
}

void FlexArray::emptyRecenter() {
    if (size_ == 0) {
        if (capacity_ % 2 == 0) {
            headroom_ = capacity_ / 2;
            tailroom_ = capacity_ / 2;
        } else {
            headroom_ = capacity_ / 2;
            tailroom_ = (capacity_ / 2) + 1;
        }
        return;
    }
}

void FlexArray::shrinkCapacity() {
    if (size_ == 0)
        return;
    if ((capacity_ / size_) >= HI_THRESHOLD) {
        capacity_ = size_ * LO_THRESHOLD;
        int* tmp = new int[capacity_];
        for (int i = 0; i < size_; i++){
            tmp[size_ + i] = arr_[headroom_ + i];
        }
        headroom_ = size_;
        tailroom_ = size_;
        delete [] arr_;
        arr_ = tmp;
    }
}

void FlexArray::expandCapacity() {
    capacity_ = size_ * LO_THRESHOLD;
    int* tmp = new int[capacity_];
    for (int i = 0; i < size_; i++){
        tmp[size_ + i] = arr_[headroom_ + i];
    }
    headroom_ = size_;
    tailroom_ = size_;
    delete [] arr_;
    arr_ = tmp;
}