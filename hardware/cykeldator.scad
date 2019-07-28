// Basic encosulre for the bike computer. Specail made
// For a quick n'dirty assembly of a blue-pill STM32 MCU
// with 2 buttons and a 1.3 inch display. Totally non-standard.
// 
// It is made of two parts. Comment out each one to print the other.


eps = 0.01;
wall_width = 1.5;

pcb_w = 66;
pcb_h = 52;
pcb_d = 1.5;

width = pcb_w + 2*wall_width;
height = pcb_h + 2*wall_width;

// Screen related measurements.
scr_w = 36;
scr_h = 20;

scr_to_pcb_h = 7;
scr_offs_x = 0;
scr_offs_y = (pcb_h - scr_h) / 2 - scr_to_pcb_h;

// Buttons.
button_d = 13;
bt_to_pcb_h = 11;

bt_offs_x = 24;
bt_offs_y = pcb_h / 2 - bt_to_pcb_h;

// Support height.
supp_h = 9;
supp_w = 8;

// Height for the backside.
back_d = 15;

// Top part
/*
union() {
translate([0,0, wall_width/2]) 
difference() {
// Base front plate.
translate([0,0,0]) cube([width, height, wall_width], center=true);
// Opening display.
translate([scr_offs_x, scr_offs_y,0]) cube([scr_w,scr_h,wall_width+eps], center = true);
// Button 1 hole.
translate([bt_offs_x, bt_offs_y,0]) cylinder(wall_width+eps, button_d/2, button_d/2, center = true, $fn=32);
// Button 2 hole.
translate([-bt_offs_x, bt_offs_y,0]) cylinder(wall_width+eps, button_d/2, button_d/2, center = true, $fn=32);
}
translate([0,0, supp_h/2+wall_width]) {
// low Support 1
translate([-(pcb_w-supp_w)/2, -(pcb_h-supp_w)/2,0]) cube([supp_w+eps, supp_w+eps, supp_h+eps], center=true);
// low Support 2
translate([(pcb_w-supp_w)/2,-(pcb_h-supp_w)/2,0]) cube([supp_w+eps, supp_w+eps, supp_h+eps], center=true);
}
translate([0,0, supp_h/2+wall_width/2]) {
// top_sidewall
translate([0,(pcb_h+wall_width)/2,0]) cube([width, wall_width, wall_width + supp_h], center=true);
// bottom_sidewall
translate([0,-(pcb_h+wall_width)/2,0]) cube([width, wall_width, wall_width + supp_h], center=true);
}
side_z = wall_width + supp_h + pcb_d + back_d;
translate([0,0, side_z / 2]) {
// left_sidewall
translate([-(pcb_w+wall_width)/2,0,0]) cube([wall_width, height, side_z], center=true);
// right_sidewall
translate([(pcb_w+wall_width)/2,0,0]) cube([wall_width, height, side_z], center=true);
}
}
*/

// Bottom part 
translate([0,0, wall_width/2]) 
union() {
//translate([0,0, wall_width/2]) 

// Base front plate.
translate([0,0,0]) cube([width, height, wall_width], center=true);

// Support ext conn
supp1_h=11.5;
supp1_w=6;
supp1_x=pcb_w/2 - 20;
supp1_y=pcb_h/2 - 10;    
translate([supp1_x, supp1_y, (
    supp1_h + wall_width)/2]) cube([supp1_w+eps, supp1_w+eps, supp1_h], center=true);

supp2_w=6;    
supp2_h=15;    
translate([0,0, supp2_h/2+wall_width/2]) {
// low Support 1
translate([-(pcb_w-supp2_w)/2, -(pcb_h-supp2_w)/2,0]) cube([supp2_w+eps, supp2_w+eps, supp2_h+eps], center=true);
// low Support 2
translate([(pcb_w-supp2_w)/2,-(pcb_h-supp2_w)/2,0]) cube([supp2_w+eps, supp2_w+eps, supp2_h+eps], center=true);
// high Support 1
translate([-(pcb_w-supp2_w)/2,(pcb_h-supp2_w)/2,0]) cube([supp2_w+eps, supp2_w+eps, supp2_h+eps], center=true);
    
}

side_z = 16;
translate([0,0, side_z / 2 + wall_width/2]) {
// up_sidewall
translate([0,-(pcb_h+wall_width)/2,0]) cube([pcb_w, wall_width, side_z], center=true);
// down_sidewall
translate([0,(pcb_h+wall_width)/2,0]) cube([pcb_w, wall_width, side_z], center=true);
}
}
