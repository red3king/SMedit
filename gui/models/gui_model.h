#pragma once

#include "models/entity.h"
#include "models/project.h"
#include "gui/draw_context.h"
#include "gui/current_events.h"
#include "signals.h"


#define NOTIF_HEIGHT 22.0
#define NOTIF_WIDTH 22.0
#define NOTIF_GAP 6.0

#define NOTIF_FONT_SIZE 16
#define NOTIF_TEXT_WIDTH 220
#define NOTIF_TEXT_OFFSET 3


class GMNotification
{
    public:
        GMNotification(DrawContext *ctx, int icon, string message);
        
        int icon;
        int id;
        string message;
        bool selected;
        float world_x, world_y;
        
        void set_coordinates(float x, float y);
        void check_mouse_within(float mouse_x, float mouse_y);

        void draw();
        void draw_message();

        static int _id_ctr;

    private:
        DrawContext *ctx;
};


class GUIModel
{
    public:
        GUIModel(DrawContext *ctx, EntityType type);
        virtual ~GUIModel()=0;

        void draw();
        virtual void draw_impl()=0;
        virtual Entity* get_entity()=0;

        virtual CursorType update_impl(CurrentEvents& current_events)=0;
        virtual bool mouse_within(float mouse_x, float mouse_y)=0;
        virtual bool too_small()=0;

        virtual void get_notification_coordinates(float& world_x, float& world_y)=0;
        void draw_notifications();

        CursorType update(CurrentEvents& current_events, GUIModel*& just_selected);
        void update_notifications(CurrentEvents& current_events);
        unsigned int get_id();

        void set_selected(bool selected);

        EntityType type;

    protected:
        DrawContext *ctx;
        bool selected, mouse_over;
        vector<GMNotification> notifications;
        
        int add_notification(GMNotification note);  // returns notification id
        void remove_notification(int note_id);      // remove note by id
};

