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

        void on_close_click()
        {
            std::cout << "closing";
            hide();
        }

    private:
        Glib::RefPtr<Gtk::Builder> builder;
};


int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "de.engelmarkus.example");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("mainwin", wnd);

    Gtk::Button* closeBtn = nullptr;
    builder->get_widget("closebutton", closeBtn);
    closeBtn->signal_clicked().connect(sigc::mem_fun(wnd, &MainWindow::on_close_click));

    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

