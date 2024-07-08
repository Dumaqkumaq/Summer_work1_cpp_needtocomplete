#include <iostream>
#include <SFML/Graphics.hpp>
#include<string>
#include<ctime>
#include <vector>
//-----------------

//узел для quadtree
struct rect{
    float x,y,width, height;
    //конструкторы
    rect() : x(0),y(0),width(0),height(0) {}
    rect(float x1, float y1, float w, float h) : x(x1),y(y1),width(w), height(h){}
    
    //для logic удобства
    //внутри ячейки
    bool contain(float bx, float by){
        return (bx >= x && bx <= x + width && by >= y && by <= y + height);
    }
    //пересечения с ячейкой
    bool intersects(rect& other_rect){
        return !(other_rect.x > x + width || other_rect.x + other_rect.width < x || 
                 other_rect.y > y + height || other_rect.y + other_rect.height < y);
    }
};
//существо
struct body : sf::RectangleShape{
    //территория зрения(для визуализации)
    sf::RectangleShape area;
    //тип существа
    std::string type = "basic";
    //айди для отслеживания
    int id = 0;
    //конструктор
    body( sf::Color color, std::string str, int num) : sf::RectangleShape({20,20}){
        type = str;
        id = num;

        this->setFillColor(color);
        this->setOrigin({10,10});
        this->setPosition({1000,1000});
        
        area.setSize({450,450});
        area.setOrigin({this->area.getSize().x/2,this->area.getSize().x/2});
        area.setPosition(this->getPosition().x,this->getPosition().y);
        area.setOutlineColor(sf::Color::Red);
        area.setFillColor(sf::Color::Transparent);
        area.setOutlineThickness(1);
    }
    //для работы в векторах
    bool operator == (body& bd){
        return ((this->getPosition().x == bd.getPosition().x) && (this->id == bd.id));
    }
    bool operator == (const body& bd) const {
        return ((this->getPosition().x == bd.getPosition().x) && (this->id == bd.id));
    }
    //сеттер типа
    void set_type(std::string str){
        this->type = str;
    }
    //нахождения поведения по отношению к другому существу
    int check_type(body& bd){
        // поведение хищиника -> 2
        // поведение жертвы -> 1
        if(this->type == "stone"){
            if(bd.type == "scissors"){
                return 2;
            } else if(bd.type == "paper") {
                return 1;
            }
        } else if(this-> type == "paper"){
            if(bd.type == "stone"){
                return 2;
            } else if(bd.type == "scissors") {
                return 1;
            }
        } else if(this-> type == "scissors"){
            if(bd.type == "paper"){
                return 2;
            } else if(bd.type == "stone") {
                return 1;
            }
        }
        return 0;
    }
    //проверка видимости другого\обнаружения другим
    bool check_intersect(body& en){
        return area.getGlobalBounds().intersects(en.getGlobalBounds());
    }
    //превращение в нападающего(проверка песечений существ внутри функции)
    void turn_en(body& en){
        if(this->getGlobalBounds().intersects(en.getGlobalBounds())){
                    std::cout<<this->id<<" dead from "<<en.id<<std::endl;
                    this->type = en.type;
                    this->setFillColor(en.getFillColor());
                }
    }
    //превращение жертвы
    void food_turn(body& en){
        if(this->getGlobalBounds().intersects(en.getGlobalBounds())){
                    std::cout<<this->id<<" hunted for "<<en.id<<" "<<en.type<<std::endl;
                    en.set_type(this->type);
                    en.setFillColor(this->getFillColor());
                }
    }
    //передвижение
    void moving(int x, int y, body& bd){
        if(check_intersect(bd)){
            //доля случайности в передвижении
            int rnd = rand()%6-1;
            //проверка типа поведения
            //run away
            if(check_type(bd) == 1){
                if(((this->getPosition().x - bd.getPosition().x) > 0) && ((this->getPosition().y - bd.getPosition().y) >= 0)){
                    x = abs(x+rnd);
                    y = abs(y+rnd);
                } else if(((this->getPosition().x - bd.getPosition().x) <= 0) && ((this->getPosition().y - bd.getPosition().y) > 0)){
                    x = abs(x+rnd)*(-1);
                    y = abs(y+rnd);
                } else if(((this->getPosition().x - bd.getPosition().x) < 0) && ((this->getPosition().y - bd.getPosition().y) <= 0)){
                    x = abs(x+rnd)*(-1);
                    y = abs(y+rnd)*(-1);
                } else if(((this->getPosition().x - bd.getPosition().x) >= 0) && ((this->getPosition().y - bd.getPosition().y) < 0)){
                    x = abs(x+rnd);
                    y = abs(y+rnd)*(-1);
                }
                std::cout<<this->id<<" runing away from "<<bd.id<<std::endl;
                turn_en(bd);
            } 
            //hunting
            else if (check_type(bd) == 2){
                if(((this->getPosition().x - bd.getPosition().x) >= 0) && ((this->getPosition().y - bd.getPosition().y) >= 0)){
                    x = abs(x+rnd)*(-1);
                    y = abs(y+rnd)*(-1);
                } else if(((this->getPosition().x - bd.getPosition().x) < 0) && ((this->getPosition().y - bd.getPosition().y) >= 0)){
                    x = abs(x+rnd);
                    y = abs(y+rnd)*(-1);
                } else if(((this->getPosition().x - bd.getPosition().x) <= 0) && ((this->getPosition().y - bd.getPosition().y) < 0)){
                    x = abs(x+rnd);
                    y = abs(y+rnd);
                } else if(((this->getPosition().x - bd.getPosition().x) > 0) && ((this->getPosition().y - bd.getPosition().y) <= 0)){
                    x = abs(x+rnd)*(-1);
                    y = abs(y+rnd);
                }
                std::cout<<this->id<<" hunting for "<<bd.id<<std::endl;
                food_turn(bd);
            }
        }

        //idle х
        if((this->getPosition().x+x) > (600 - this->getSize().x / 2)){
            x = x - this->getSize().x / 2 - (this->getPosition().x+x - 600);
        } else if((this->getPosition().x+x) < (this->getSize().x / 2)){
            x = this->getPosition().x - this->getSize().x / 2;
        }
        //idle у
        if((this->getPosition().y+y) > (600 - this->getSize().x / 2)){
            y = y - this->getSize().y / 2 - (this->getPosition().y+y - 600);
        } else if((this->getPosition().y+y) < (this->getSize().x / 2)){
            y = this->getPosition().y - this->getSize().x / 2;
        }
        //moving
        area.setPosition(this->getPosition().x+x,this->getPosition().y+y);
        this->setPosition(this->getPosition().x+x,this->getPosition().y+y);
    }
};

//подразделение пространства методом древа\quadtree
struct quadtree{
    //ячейка
    rect boundary;
    //вместимость
    int capacity;
    //поделен или нет
    bool divided;
    //массив существ внутри ячейки
    std::vector<body> bodys;
    /*
        массив подузлов
        nw -> northwest
        ne -> northeast
        sw -> southwest
        se -> southeast
    */
    std::unique_ptr<quadtree> nw,ne,sw,se;
    //конструктор
    quadtree(rect& boun, int cap) 
        : boundary(boun), capacity(cap), divided(false) {}
    //функция отрисовки
    void draw(sf::RenderWindow &window){
        sf::RectangleShape rectangle({boundary.width,boundary.height});
        rectangle.setPosition(boundary.x,boundary.y);
        rectangle.setOutlineColor(sf::Color::White);
        rectangle.setOutlineThickness(1);
        rectangle.setFillColor(sf::Color::Transparent);
        window.draw(rectangle);
        if(divided){
            nw->draw(window);
            ne->draw(window);
            sw->draw(window);
            se->draw(window);
        } 
        
    }
    //деление узла на подузлы
    void subdivide(){
        
        nw = std::make_unique<quadtree>(rect(boundary.x,boundary.y,boundary.width/2,boundary.height/2),capacity);
        ne = std::make_unique<quadtree>(rect(boundary.x+boundary.width/2,boundary.y,boundary.width/2,boundary.height/2),capacity);
        sw = std::make_unique<quadtree>(rect(boundary.x,boundary.y+boundary.height/2,boundary.width/2,boundary.height/2),capacity);
        se = std::make_unique<quadtree>(rect(boundary.x+boundary.width/2,boundary.y+boundary.height/2,boundary.width/2,boundary.height/2),capacity);


        this->divided = true;
    }
    //объединение подузлов
    void merge(){
        if(!divided) return;
        auto mergeBodies = [this](const std::unique_ptr<quadtree>& qt) {
            bodys.insert(bodys.end(),qt->bodys.begin(), qt->bodys.end());
        };

        mergeBodies(nw);
        mergeBodies(ne);
        mergeBodies(sw);
        mergeBodies(se);

        nw.reset();
        ne.reset();
        sw.reset();
        se.reset();

        divided = false;
    }
    //вставка существа в узел\подузел
    bool insert(body& bd){
        //если тело внутри
        if(!boundary.contain(bd.getPosition().x,bd.getPosition().y)){
            return false;
        }
        //если есть место, то добавляем 
        if(int(bodys.size()) < this->capacity){
            bodys.push_back(bd); 
            return true;
        }
        //места нет -> делим узел
        if(!divided ){
            subdivide();
        }
        //вставляем точку в подузел
        if(nw->insert(bd)) return true;
        if(ne->insert(bd)) return true;
        if(sw->insert(bd)) return true;
        if(se->insert(bd)) return true;

        return false;
    }
    //геттер кол-ва существ в узле и его подузлах
    int totalBodies(){
            int total = this->bodys.size();
            if(divided){
                total += nw->totalBodies();
                total += ne->totalBodies();
                total += sw->totalBodies();
                total += se->totalBodies();
            }
            return total;
    }
    //удаление существа из узла\подузла
    void remove(body& bd){
        auto it = std::find(bodys.begin(),bodys.end(),bd);
        if(it != bodys.end()){
            bodys.erase(it);
            return;
        }
        if(divided){
            nw->remove(bd);
            ne->remove(bd);
            sw->remove(bd);
            se->remove(bd);
        }
        if(divided && (totalBodies() < capacity)) merge();
    }
    //нахождение существ в зоне видимости range(ячейка)
    //строка type для оптимизации определения хищников\жертв
    void query(rect& range, std::vector<body>& found, std::string type) {
        if(!boundary.intersects(range)) return;

        for(body& bd : bodys){
            if(range.contain(bd.getPosition().x,bd.getPosition().y) && bd.type != type) 
                found.push_back(bd);
        }

        if(divided){
            nw->query(range,found,type);
            ne->query(range,found,type);
            sw->query(range,found,type);
            se->query(range,found,type);
        }
    }
};

int main(int, char**){
    srand(time(0));

//-----------------
    //для подсчета существ
    int stone = 40;
    int paper = 40;
    int scissors = 40;
    int all = scissors + stone + paper;;
    //второстепенные нужды
    std::string str;
    int global_id = 0;
    //для подразделения пространства и работы с ним
    rect vis(0,0,450,450);
    rect g(0,0,600,600);
    quadtree qt(g,2);
    //общий массив существ
    std::vector<body> bds;

//-----------------
    //создание существ с определенным типом

    //paper
    for(int i = 0; i < paper; i++){
        global_id++;
        bds.emplace_back(sf::Color::White, "paper",global_id);
        qt.insert(bds.back());
        bds.back().setPosition(100,100);
    }
    //stone
    for(int i = 0; i < stone; i++){
        global_id++;
        bds.emplace_back(sf::Color::Green, "stone",global_id);
        qt.insert(bds.back());
        bds.back().setPosition(500,100);
    }
    //scissors
    for(int i = 0; i < scissors; i++){
        global_id++;
        bds.emplace_back(sf::Color::Red, "scissors",global_id);
        qt.insert(bds.back());
        bds.back().setPosition(300,450);
    }
    

//-----------------
    
    sf::RenderWindow window(sf::VideoMode(600,600), "uwu");

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed) window.close();

        }

        window.clear();
        //вывод кол-во существ в заголовке окна(для удобства)
        str = "all: "+ std::to_string(all) +"  stone: "+std::to_string(stone);
        str = str + " paper: "+ std::to_string(paper) + " scissors: "+ std::to_string(scissors);
        window.setTitle(str);

        all = scissors + stone + paper;
        scissors = 0;
        stone = 0;
        paper = 0;

        //обработка передвижения
        for(body& bd : bds){
            //подсчет существ
            if(bd.type == "stone") stone ++;
            else if(bd.type == "paper") paper++;
            else if(bd.type == "scissors") scissors++;
            
            //проверка на наличие существа в узле
            qt.remove(bd);
            //массив видимых существ
            std::vector<body> in_area;
            //передвигаем ячейку видимости\зрения
            vis.x = bd.getPosition().x-150;
            vis.y = bd.getPosition().y-150;
            //получаем массив видимых существ
            qt.query(vis,in_area,bd.type);

            //проверяем видимых существ (если никого нет, то добавляем само существо)
            if(in_area.empty()) in_area.push_back(bd);
            //определяем поведение для данного цикла
            for(auto& bd2 : in_area) {
                std::cout<<bd.id<<"->\t"<<bd2.id<<"  "; //вывод видимых для удобства
               if(bd.check_type(bd2) == 1) {
                in_area[0] = bd2;
                bd.turn_en(bd2);
               }
               else if(bd.check_type(bd2) == 2) {
                in_area[0] = bd2;
                bd.food_turn(bd2);
               }
            }
            //передвижение существа
            bd.moving(5*pow((-1),rand()%2),5*pow((-1),rand()%2),in_area[0]);
            std::cout<<std::endl;

            //проверка на вход в ячейку существом
            qt.insert(bd);
        }

        //отрисовка существ
        for(auto& bd : bds){
            // window.draw(bd.area);
            window.draw(bd);
        }

        //отрисовка подразделения пространства(древа)
        qt.draw(window);
        

        window.display();
    }

    return 0;
}