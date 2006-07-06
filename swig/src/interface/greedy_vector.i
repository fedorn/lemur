#ifdef SWIGCSHARP

/* clone the std_vector.i for greedy_vector
 */
%include <std_common.i>

// MACRO for use within the indri::utility::greedy_vector class body
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_GREEDY_VECTOR_MINIMUM(CSTYPE, CTYPE...)
%typemap(csinterfaces) indri::utility::greedy_vector<CTYPE > "IDisposable, System.Collections.IEnumerable";
%typemap(cscode) indri::utility::greedy_vector<CTYPE > %{
  public $csclassname(System.Collections.ICollection c) : this() {
    if (c == null)
      throw new ArgumentNullException("c");
    foreach (CSTYPE element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public CSTYPE this[int index]  {
    get {
      return getitem(index);
    }
    set {
      setitem(index, value);
    }
  }

  public int Capacity {
    get {
      return (int)capacity();
    }
    set {
      if (value < size())
        throw new ArgumentOutOfRangeException("Capacity");
      reserve((uint)value);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(System.Array array) {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(System.Array array, int arrayIndex) {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, System.Array array, int arrayIndex, int count) {
    if (array == null)
      throw new ArgumentNullException("array");
    if (index < 0)
      throw new ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new ArgumentException("Multi dimensional array.");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  // Type-safe version of IEnumerable.GetEnumerator
  System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
    return new $csclassnameEnumerator(this);
  }

  public $csclassnameEnumerator GetEnumerator() {
    return new $csclassnameEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class $csclassnameEnumerator : System.Collections.IEnumerator {
    private $csclassname collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public $csclassnameEnumerator($csclassname collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public CSTYPE Current {
      get {
        if (currentIndex == -1)
          throw new InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new InvalidOperationException("Collection modified.");
        return (CSTYPE)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new InvalidOperationException("Collection modified.");
      }
    }
  }
%}

  public:
    typedef CTYPE* iterator;
    typedef const CTYPE* const_iterator;
    typedef CTYPE& reference;
    typedef const CTYPE& const_reference;
    %rename(Clear) clear;
    void clear();
    %rename(Add) push_back;
    void push_back(CTYPE& value);
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t n);
    %newobject GetRange(int index, int count);
    %newobject Repeat(const CTYPE& value, int count);
greedy_vector();
    %extend {
      greedy_vector(int capacity) throw (std::out_of_range) {
        indri::utility::greedy_vector<CTYPE >* pv = 0;
        if (capacity >= 0) {
          pv = new indri::utility::greedy_vector<CTYPE >();
          pv->reserve(capacity);
       } else {
          throw std::out_of_range("capacity");
       }
       return pv;
      }
      CTYPE getitemcopy(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size())
          return (*self)[index];
        else
          throw std::out_of_range("index");
      }
      const_reference getitem(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size())
          return (*self)[index];
        else
          throw std::out_of_range("index");
      }
      void setitem(int index, const CTYPE& value) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size())
          (*self)[index] = const_cast<CTYPE &>(value);
        else
          throw std::out_of_range("index");
      }
      // Takes a deep copy of the elements unlike ArrayList.AddRange
      void AddRange(const indri::utility::greedy_vector<CTYPE >& values) {
        self->append(values.begin(), values.end());
      }
      // Takes a deep copy of the elements unlike ArrayList.GetRange
      indri::utility::greedy_vector<CTYPE > *GetRange(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        indri::utility::greedy_vector<CTYPE > *res = new indri::utility::greedy_vector<CTYPE >((size_t) count);
        res->assign(self->begin()+index, self->begin()+index+count);
        return res;
      }
      void Insert(int index, const CTYPE& value) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()+1)
          // insert x before pos
          // violates insert semantics.
          // FIXME
          self->push_back(const_cast<CTYPE &>(value));
        else
          throw std::out_of_range("index");
      }
      // Takes a deep copy of the elements unlike ArrayList.InsertRange
      void InsertRange(int index, const indri::utility::greedy_vector<CTYPE >& values) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()+1)
          // violates insert semantics.
          //Inserts the range [first, last) before pos.
          //FIXME
          self->append(values.begin(), values.end());
        else
          throw std::out_of_range("index");
      }
      void RemoveAt(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size())
          self->erase(self->begin() + index);
        else
          throw std::out_of_range("index");
      }
      void RemoveRange(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        self->erase(self->begin()+index, self->begin()+index+count);
      }
      static indri::utility::greedy_vector<CTYPE > *Repeat(const CTYPE& value, int count) throw (std::out_of_range) {
        if (count < 0)
          throw std::out_of_range("count");
        indri::utility::greedy_vector<CTYPE > *res = new indri::utility::greedy_vector<CTYPE >();
        for (int i = 0; i < count; i++) {
          res->push_back( const_cast<CTYPE &>(value) );
        }
        return res;        
      }
      void Reverse() {
        std::reverse(self->begin(), self->end());
      }
      void Reverse(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        std::reverse(self->begin()+index, self->begin()+index+count);
      }
      // Takes a deep copy of the elements unlike ArrayList.SetRange
      void SetRange(int index, const indri::utility::greedy_vector<CTYPE >& values) throw (std::out_of_range) {
        if (index < 0)
          throw std::out_of_range("index");
        if (index+values.size() > self->size())
          throw std::out_of_range("index");
        std::copy(values.begin(), values.end(), self->begin()+index);
      }
    }
%enddef

// Extra methods added to the collection class if operator== is defined for the class being wrapped
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_GREEDY_VECTOR_EXTRA_OP_EQUALS_EQUALS(CSTYPE, CTYPE...)
    %extend {
      bool Contains(const CTYPE& value) {
        return std::find(self->begin(), self->end(), value) != self->end();
      }
      int IndexOf(const CTYPE& value) {
        int index = -1;
        indri::utility::greedy_vector<CTYPE >::iterator it = std::find(self->begin(), self->end(), value);
        if (it != self->end())
          index = it - self->begin();
        return index;
      }
      int LastIndexOf(const CTYPE& value) {
        int index = -1;
        indri::utility::greedy_vector<CTYPE >::reverse_iterator rit = std::find(self->rbegin(), self->rend(), value);
        if (rit != self->rend())
          index = self->rend() - 1 - rit;
        return index;
      }
      void Remove(const CTYPE& value) {
        indri::utility::greedy_vector<CTYPE >::iterator it = std::find(self->begin(), self->end(), value);
        if (it != self->end())
          self->erase(it);
      }
    }
%enddef

// Macros for indri::utility::greedy_vector class specializations
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_GREEDY_VECTOR_SPECIALIZE(CSTYPE, CTYPE...)
namespace indri {
  namespace utility {
    template<> class greedy_vector<CTYPE > {
      SWIG_GREEDY_VECTOR_MINIMUM(CSTYPE, CTYPE)
        SWIG_GREEDY_VECTOR_EXTRA_OP_EQUALS_EQUALS(CSTYPE, CTYPE)
        };
  }  
}
%enddef

%define SWIG_GREEDY_VECTOR_SPECIALIZE_MINIMUM(CSTYPE, CTYPE...)
namespace indri {
  namespace utility {
  template<> class greedy_vector<CTYPE > {
    SWIG_GREEDY_VECTOR_MINIMUM(CSTYPE, CTYPE)
      };
  }
}

%enddef


%{
#include "indri/greedy_vector"
#include <algorithm>
#include <stdexcept>
%}

%csmethodmodifiers indri::utility::greedy_vector::getitemcopy "private"
%csmethodmodifiers indri::utility::greedy_vector::getitem "private"
%csmethodmodifiers indri::utility::greedy_vector::setitem "private"
%csmethodmodifiers indri::utility::greedy_vector::size "private"
%csmethodmodifiers indri::utility::greedy_vector::capacity "private"
%csmethodmodifiers indri::utility::greedy_vector::reserve "private"

namespace indri {
  namespace utility {
  template<class T> class greedy_vector {
    SWIG_GREEDY_VECTOR_MINIMUM(T, T)
  };
  // specializations for pointers
  template<class T> class greedy_vector<T*> {
    SWIG_GREEDY_VECTOR_MINIMUM(T, T*)
  };
  template<class T> class greedy_vector<const T*> {
    SWIG_GREEDY_VECTOR_MINIMUM(T, const T*)
  };
  }
}


#endif
