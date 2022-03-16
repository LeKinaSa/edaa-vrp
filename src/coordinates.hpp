
class Coordinates {
    public:
        Coordinates(double latitude, double longitude);

        double getLatitude() const;
        double getLongitude() const;

        void setLatitude(double latitude);
        void setLongitude(double longitude);

        double haversine(const Coordinates& other) const;
    private:
        double latitude, longitude;
};
