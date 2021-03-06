#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

class Camera {

    private:

        sf::View view;

        //default view size that way camera can revert any zooms
        //the default view size is always equal ot the windows current resolution
        //so whenever the window's resolution is changed you should change the default view size
        sf::Vector2f defaultCameraSize;

        //the current viewing area of the camera
        //this value will move towards the destination camera size in order to have smooth zooming
        sf::Vector2f currentCameraSize;

        sf::Vector2f destinationCameraSize;

        //timer used to calculate how much to zoom every time the camera is updated
        sf::Clock transitionTimer;

        //how much camera zooms in or out every time the zoom function is called
        const float ZOOM_FACTOR;

        //apply the zoom level to the camera
        void applyZoom();

        //calculate the spped at which the camera transitions from the current size to the destination size
        //rate is pixels/second
        const sf::Vector2f calculateZoomVelocity() const ;

        //move from the current zoom towards the destination zoom
        void transitionZoom();

        //forcibly resets all sizes
        void resetAll();

    public:

        Camera();

        //set the view to the given window
        void applyCamera(sf::RenderWindow& window);

        //set the default view to the given window
        void applyDefaultCamera(sf::RenderWindow& window) const;

        //get the viewing rect of the camera
        const sf::FloatRect getCameraBounds() const;

        //get the viewing rect of the camera if the camera was centered around the given position
        const sf::FloatRect getCameraBounds(const sf::Vector2f& cameraCenter) const;

        //set the new center position of the camera, the center of the center is basically the center of the target given
        //the center position must be within the given world bounds
        //camera will also ensure that it doesn't leave the world bounds
        void setCameraCenter(const sf::Vector2f& targetCenter, const sf::Vector2f& worldBounds);

        //take the window and use it's dimensions to determine the new default size
        //resets zoom level
        void setDefaultSize(sf::RenderWindow& window);

        ///everything that changes the zoom level makes the camera zoom smoothly, meaning the zoom doesn't happen instantly and it will take time
        void zoomIn();
        void zoomOut();

        void resetZoom();
};

#endif // CAMERA_H_INCLUDED
