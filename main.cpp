#include <iostream>
#include <memory>
#include <gtkmm.h>

#include <giomm/resource.h>
#include <epoxy/gl.h>

#define NANOVG_GL3_IMPLEMENTATION
#include "lib/nanovg/nanovg.h"
#include "lib/nanovg/nanovg_gl.h"

// for string shader loading stuff
#include <string>
#include <fstream>
#include <streambuf>



class MainWindow : public Gtk::ApplicationWindow 
{
    public:
    
        MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , 
            builder{builder}
        {
            set_title("smedit2");
                
            Gtk::Button* closeBtn = nullptr;
            builder->get_widget("closebutton", closeBtn);
            closeBtn->signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_close_click));
            
            Gtk::Button* rotate_button = nullptr;
            builder->get_widget("rotatebutton", rotate_button);
            rotate_button->signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_rotate_click));

            // GLArea stuff
            gl_area = nullptr;
            builder->get_widget("gl_area", gl_area);


            gl_area->signal_realize().connect(sigc::mem_fun(this, &MainWindow::on_gl_realize));
            gl_area->signal_unrealize().connect(sigc::mem_fun(this, &MainWindow::on_gl_unrealize), false);
            gl_area->signal_render().connect(sigc::mem_fun(this, &MainWindow::on_gl_render), false);

        }

        virtual ~MainWindow() = default;


        Gtk::GLArea* gl_area;
        NVGcontext* vg;

        bool compile_shader(GLuint& shader, int shader_type, std::string shader_path)
        {
            // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
            std::ifstream st(shader_path);
            std::string shader_data((std::istreambuf_iterator<char>(st)), (std::istreambuf_iterator<char>()));
            const char* c_str = shader_data.c_str();

            shader = glCreateShader(shader_type);
            glShaderSource(shader, 1, &c_str, NULL);
            glCompileShader(shader);
            
            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            //char buffer[512];
            //glGetShaderInfoLog(shader, 512, NULL, buffer);
            return status == GL_TRUE;
        }

        
        void on_gl_realize()
        {
            gl_area->make_current();

            vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
            // check if vg is null todo


            gl_area->throw_if_error();
        }

        void on_gl_unrealize()
        {
            gl_area->make_current();
            gl_area->throw_if_error();
        }

        bool on_gl_render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            std::cout<<"render";
            gl_area->throw_if_error();

            int width = gl_area->get_allocated_width();
            int height = gl_area->get_allocated_height();
            float px_ratio = (float) width / (float) height;
            glViewport(0, 0, width, height);

            glClearColor(0.5, 0.5, 0.5, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
            nvgBeginFrame(vg, width, height, px_ratio);

            nvgBeginPath(vg);
            nvgRoundedRect(vg, 20, 20, 100, 100, 5);
            nvgFillColor(vg, nvgRGBA(200,200,200,255));
            nvgFill(vg);

            nvgEndFrame(vg);
    
            glFlush(); 
            return true;
        }

        void on_close_click()
        {
            std::cout << "closing";
            hide();
        }

        void on_rotate_click()
        {
            gl_area->queue_draw();
        }



    private:
        Glib::RefPtr<Gtk::Builder> builder;
};


int main(int argc, char* argv[]) 
{
    auto app = Gtk::Application::create(argc, argv, "de.engelmarkus.example");
    auto builder = Gtk::Builder::create();

    builder->add_from_file("mainlayout_gl.glade");
    
    MainWindow* wnd = nullptr;
    builder->get_widget_derived("mainwin", wnd);


    auto r = app->run(*wnd);
    delete wnd;
    return r;
}

