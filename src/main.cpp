#include "../include/UserInterfaceLayer.h"

int main(){
    DataSourceLayer dl{};
    // dl.seedDatabase();
    UserInterfaceLayer ui{};
    ui.Start();
    return 0;
}
