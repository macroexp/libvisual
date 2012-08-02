#ifndef _LV_SINGLETON_HPP
#define _LV_SINGLETON_HPP

namespace LV {

  template <class T>
  class Singleton
  {
  public:

      Singleton (Singleton const&) = delete;

      Singleton const& operator= (Singleton const&) = delete;

      static void deinit ()
      {
          delete m_instance;
          m_instance = 0;
      }

      static T* instance () {
          return m_instance;
      }

  protected:

      static T* m_instance;

      Singleton () {}
  };

} // LV namespace

#endif // _LV_SINGLETON_HPP
