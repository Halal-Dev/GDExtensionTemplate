#include "SignalSystem.h"
#include "Player.hpp"
#include "Coin.hpp"

/* Player */
Player::Player()
{
    velocity = godot::Vector2();
    screensize = godot::Vector2(480, 720);
}

Player::~Player()
{

}

void Player::set_speed(float speed)
{
    this->player_speed = speed;
}

float Player::get_speed()
{
    return this->player_speed;
}

void Player::_bind_methods()
{
    /*
    godot::ClassDB::bind_method(godot::D_METHOD("set_speed", "speed"), &Player::set_speed);
    godot::ClassDB::bind_method(godot::D_METHOD("get_speed"), &Player::get_speed);

    godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::FLOAT, "speed"), "set_string", "get_string");
    */
   ADD_SIGNAL(godot::MethodInfo("pickup"));
   ADD_SIGNAL(godot::MethodInfo("hurt"));
}

void Player::ProcessMovement(double delta)
{
    this->velocity = godot::Input::get_singleton()->get_vector("ui_left", "ui_right", "ui_up", "ui_down");
    this->set_position(get_position() + this->velocity * this->player_speed * delta);

    this->set_position(
        godot::Vector2(
            godot::UtilityFunctions::clamp(this->get_position().x, 0, screensize.x),
            godot::UtilityFunctions::clamp(this->get_position().y, 0, screensize.y)
        )
    );
}

void Player::ProcessAnimations(double delta)
{
    if (this->velocity.length() > 0)
        this->sprite->set_animation("run");
    else
        this->sprite->set_animation("idle");

    if (this->velocity.x != 0)
        this->sprite->set_flip_h(velocity.x < 0);
    
};

void Player::Start()
{
    set_process(true);
    set_position(screensize/2);
    this->sprite->set_animation("idle");
}

void Player::Die()
{
    this->sprite->set_animation("hurt");
    set_process(false);
}



void Player::area_entered_callback(const godot::Variant** inArguments, int inArgcount, godot::Variant& outReturnValue, GDExtensionCallError& outCallError)
{
    godot::Object* obj = inArguments[0]->operator godot::Object *();
    if (obj->is_class("Coin"))
    {
        Coin* target = godot::Object::cast_to<Coin>(obj);
        //CoinDashGame* game = godot::Object::cast_to<CoinDashGame>(godot::Engine::get_singleton()->get_singleton("CoinDashGame"));
        
        if (target->is_in_group("coins"))
        {
            godot::UtilityFunctions::print("Hit a coin !");
            target->Pickup();
            this->emit_signal("pickup");
        } else if (target->is_in_group("obstacles"))
        {
            godot::UtilityFunctions::print("Hit an obstacle !");
            this->emit_signal("hurt");
        }
    }
    outCallError.error = GDEXTENSION_CALL_OK;
}

void Player::_ready()
{
    /*
    Callback connect test
    CallbackSignalConnector connector2;
    connector2.connect(this, "area_entered", [](const godot::Variant** inArguments, int argCount, godot::Variant& args_out, GDExtensionCallError& error){
        godot::UtilityFunctions::print("Connected fr");
    });
    */
    
    SignalConnector<Player> connector(this);
    connector.connect(this, "area_entered", &Player::area_entered_callback);
    if (!godot::Engine::get_singleton()->is_editor_hint())
    {
        this->sprite = get_node<godot::AnimatedSprite2D>("AnimatedSprite2D");

        if (this->sprite != nullptr)
        {
            godot::UtilityFunctions::print("Got sprite 2D !");
            this->is_ready = true;
        }
        else {
            godot::UtilityFunctions::print("Could not find sprite , aborting...");
        }
    }
}
void Player::_process(double delta)
{
    if (!godot::Engine::get_singleton()->is_editor_hint() && is_ready) {
        ProcessMovement(delta);
        ProcessAnimations(delta);
    }
}