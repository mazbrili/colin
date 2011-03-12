#ifndef WGV_ELEMENT_H
#define WGV_ELEMENT_H


class wgv_element
{
public:
    wgv_element() {selec = false;}
    inline const bool &isSelected() const {return selec;}
    virtual void select(bool s = true){selec = s;}
protected:
    bool selec;
};

#endif // WGV_ELEMENT_H
