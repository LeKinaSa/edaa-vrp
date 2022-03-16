
class Coordinates {
    public:
        Coordinates(double latitude, double longitude);

        double getLatitude() const;
        double getLongitude() const;

        void setLatitude(double latitude);
        void setLongitude(double longitude);
    private:
        double latitude, longitude;
};
