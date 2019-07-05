#include <iostream>
#include <memory>
#include <gtkmm.h>

#include <giomm/resource.h>
#include <epoxy/gl.h>

// for string shader loading stuff
#include <string>
#include <fstream>
#include <streambuf>

enum { X, Y, Z, NUM_AXES };


static const GLfloat triangle_vertices[] =
{
    0.f,    0.5f,    0.f,    1.f,
    0.5f,   -0.366f, 0.f,    1.f,
    -0.5f,  -0.366f, 0.f,    1.f
};



class MainWindow : public Gtk::ApplicationWindow 
{
    public:
    
        MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder) : Gtk::ApplicationWindow(obj) , 
            builder{builder}
        {
            rotation_angles = std::vector<float>(NUM_AXES, 0.0f);
            rotate_ctr = 0;
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

        int rotate_ctr;

        Gtk::GLArea* gl_area;
        GLuint vao, buffer, program, mvp;
        std::vector<float> rotation_angles;

        void init_buffers()
        {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void init_shaders()
        {
            GLuint vertex_shader, frag_shader;
            compile_shader(vertex_shader, GL_VERTEX_SHADER, "shaders/gtkmm-demo-vertex.glsl");
            compile_shader(frag_shader, GL_FRAGMENT_SHADER, "shaders/gtkmm-demo-frag.glsl");

            program = glCreateProgram();
            glAttachShader(program, vertex_shader);
            glAttachShader(program, frag_shader);
            glLinkProgram(program);

            mvp = glGetUniformLocation(program, "mvp");

            //glDeleteShader called here on both shaders in gtk demo 
            //same with glDetachShader, not sure why? maybe theyre loaded into program already?
        }

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


        
        static void compute_mvp(float *res,
                        float phi,
                        float theta,
                        float psi)
{
  float x       {phi * ((float)G_PI / 180.f)};
  float y       {theta * ((float)G_PI / 180.f)};
  float z       {psi * ((float)G_PI / 180.f)};
  float c1      {cosf (x)};
  float s1      {sinf (x)};
  float c2      {cosf (y)};
  float s2      {sinf (y)};
  float c3      {cosf (z)};
  float s3      {sinf (z)};
  float c3c2    {c3 * c2};
  float s3c1    {s3 * c1};
  float c3s2s1  {c3 * s2 * s1};
  float s3s1    {s3 * s1};
  float c3s2c1  {c3 * s2 * c1};
  float s3c2    {s3 * c2};
  float c3c1    {c3 * c1};
  float s3s2s1  {s3 * s2 * s1};
  float c3s1    {c3 * s1};
  float s3s2c1  {s3 * s2 * c1};
  float c2s1    {c2 * s1};
  float c2c1    {c2 * c1};

  /* apply all three rotations using the three matrices:
   *
   * ⎡  c3 s3 0 ⎤ ⎡ c2  0 -s2 ⎤ ⎡ 1   0  0 ⎤
   * ⎢ -s3 c3 0 ⎥ ⎢  0  1   0 ⎥ ⎢ 0  c1 s1 ⎥
   * ⎣   0  0 1 ⎦ ⎣ s2  0  c2 ⎦ ⎣ 0 -s1 c1 ⎦
   */
  res[0] = c3c2;  res[4] = s3c1 + c3s2s1;  res[8] = s3s1 - c3s2c1; res[12] = 0.f;
  res[1] = -s3c2; res[5] = c3c1 - s3s2s1;  res[9] = c3s1 + s3s2c1; res[13] = 0.f;
  res[2] = s2;    res[6] = -c2s1;         res[10] = c2c1;          res[14] = 0.f;
  res[3] = 0.f;   res[7] = 0.f;           res[11] = 0.f;           res[15] = 1.f;
}



        void draw_triangle()
        {
            float m_vp[16];
            
            compute_mvp(m_vp, rotate_ctr, 0, 0);

            glUseProgram(program);
            glUniformMatrix4fv(mvp, 1, GL_FALSE, &m_vp[0]);
            glBindBuffer(GL_ARRAY_BUFFER, vao);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUseProgram(0);    
        }

        void on_gl_realize()
        {
            gl_area->make_current();
            gl_area->throw_if_error();
            init_buffers();
            init_shaders(); 
        }

        void on_gl_unrealize()
        {
            gl_area->make_current();
            gl_area->throw_if_error();
            glDeleteBuffers(1, &vao);
            glDeleteProgram(program);
        }

        bool on_gl_render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            std::cout<<"render";
            gl_area->throw_if_error();
            glClearColor(0.5, 0.5, 0.5, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            draw_triangle();
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
            rotate_ctr += 13;
            if(rotate_ctr > 359)
                rotate_ctr -= 360;

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

