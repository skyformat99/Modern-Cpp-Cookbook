#pragma once

#include <memory>
#include <vector>

namespace recipe_10_06 {
  namespace demo1 {
    template <class T>
    class base {
    public:
      void func1()
      {
        static_cast<T*>(this)->func1();
      }
    };

    class derived : public base<derived> {
    public:
      void func1()
      {
        std::cout << "derived::func1" << std::endl;
      }
    };

    template <class T>
    void do_something(base<T>& b)
    {
      b.func1();
    }
  }

  namespace demo2 {
    template <class T>
    class control {
    public:
      void draw()
      {
        static_cast<T*>(this)->erase_background();
        static_cast<T*>(this)->paint();
      }
    };

    class button : public control<button> {
    private:
      friend class control<button>;

      void erase_background()
      {
        std::cout << "erasing button background..." << std::endl;
      }

      void paint()
      {
        std::cout << "painting button..." << std::endl;
      }
    };

    class checkbox : public control<checkbox> {
    public:
      void erase_background()
      {
        std::cout << "erasing checkbox background..." << std::endl;
      }

      void paint()
      {
        std::cout << "painting checkbox..." << std::endl;
      }
    };

    template <class T>
    void draw_control(control<T>& c)
    {
      c.draw();
    }

    template <class T>
    class fancybutton : public control<fancybutton<T>> {
    public:
      void erase_background()
      {
        static_cast<T*>(this)->paint_area();
      }

      void paint()
      {
        std::cout << "painting button..." << std::endl;
      }
    };

    class transparentbutton : public fancybutton<transparentbutton> {
    public:
      void paint_area()
      {
        std::cout << "painting transparent button background..." << std::endl;
      }
    };
  }

  namespace demo3 {
    class controlbase {
    public:
      virtual void draw() = 0;
      virtual ~controlbase() {}
    };

    template <class T>
    class control : public controlbase {
      T* derived()
      {
        return static_cast<T*>(this);
      }

    public:
      virtual void draw() override
      {
        derived()->erase_background();
        derived()->paint();
      }
    };

    class button : public control<button> {
    public:
      void erase_background()
      {
        std::cout << "erasing button background..." << std::endl;
      }

      void paint()
      {
        std::cout << "painting button..." << std::endl;
      }
    };

    class checkbox : public control<checkbox> {
    public:
      void erase_background()
      {
        std::cout << "erasing checkbox background..." << std::endl;
      }

      void paint()
      {
        std::cout << "painting checkbox..." << std::endl;
      }
    };

    void draw_controls(std::vector<std::shared_ptr<controlbase>>& v)
    {
      for (auto& c : v) {
        c->draw();
      }
    }
  }

  void execute()
  {
    {
      using namespace demo1;

      derived d;
      d.func1();

      do_something(d);
    }

    {
      using namespace demo2;

      button b;
      b.draw();
      checkbox c;

      draw_control(b);
      draw_control(c);
    }

    {
      using namespace demo2;

      transparentbutton b;
      b.draw();
    }

    {
      using namespace demo3;

      std::vector<std::shared_ptr<controlbase>> v;
      v.emplace_back(std::make_shared<button>());
      v.emplace_back(std::make_shared<checkbox>());

      draw_controls(v);
    }
  }
}
