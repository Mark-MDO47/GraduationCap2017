
module MdoRoundItOnXaxis(dim_long, hole_diam, square_width)
{
    difference() {
        translate([-dim_long/2, -square_width/2, -square_width/2]) cube([dim_long, square_width, square_width]);
        rotate([0, 90, 0]) translate([0, 0, -(dim_long+2)/2])cylinder(r=hole_diam/2,h=dim_long+2,$fn=128);
        translate([-dim_long/2-1, -square_width/2-1, -square_width-2]) cube([dim_long+2, square_width+2, square_width+2]);
    }
}   // end MdoRoundItOnXaxis()

MdoRoundItOnXaxis(100,10,15);
