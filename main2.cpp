// g++ -std=c++14 -o gtkmm-example gtkmm-example.cpp `pkg-config --cflags --libs gtkmm-3.0`

#include <iostream>
#include <memory>
#include <gtkmm.h>

class MainWindow : public Gtk::ApplicationWindow 
{
    public:
    
        MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , 
            builder{builder}
        {
        }

        virtual ~MainWindow() = default;

    private:
        Glib::RefPtr<Gtk::Builder> builder;
};


void on_close_click()
{
    std::cout << "closing\n";
    Gtk::Main::quit();
}


int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "de.engelmarkus.example");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("mainwin", wnd);

    Gtk::Button* closeBtn = nullptr;
    builder->get_widget("closebutton", closeBtn);
    closeBtn->signal_clicked().connect(sigc::ptr_fun(&on_close_click));
    
    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

