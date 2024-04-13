
l = 40.0;
b = 57.5;
d = 4.0;


bout = b + 2*d;

difference(){
  hull() {
    translate([-bout/2,-l/2,-d])cube([bout,l,d]);
      
    translate([-40/2,-l/2,40])cube([40,l,d]);
    
  }
 
  translate([ 0,30,20])rotate([90,0,0])cylinder(h=60, r=33/2,$fn=30);
  
  translate([-60/2,-80/2,33/2+3])cube([60,80,100]);
 // translate([-100/2,-100/2,33/2+3-100])cube([100,100,100]);

  
  translate([ 0,0,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ 0,10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ 0,-10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  
  translate([ -15,0,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ -15,10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ -15,-10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  
  translate([ 15,0,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ 15,10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  translate([ 15,-10,-5])rotate([0,0,0])cylinder(h=70, r=1.5,$fn=30);
  
  
  translate([ 0,0,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ 0,10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ 0,-10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  
  translate([ -15,0,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ -15,10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ -15,-10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  
  translate([ 15,0,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ 15,10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  translate([ 15,-10,2])rotate([0,0,0])cylinder(h=36, r=4,$fn=30);
  
  translate([ 22,+13,-20])rotate([0,0,0])cylinder(h=80, r=1.1,$fn=30);
  translate([ 22,-13,-20])rotate([0,0,0])cylinder(h=80, r=1.1,$fn=30);
  
  translate([ -22,+13,-20])rotate([0,0,0])cylinder(h=80, r=1.1,$fn=30);
  translate([ -22,-13,-20])rotate([0,0,0])cylinder(h=80, r=1.1,$fn=30);
  
  translate([ -22,-13,22])rotate([0,0,0])cylinder(h=80, r=3.5,$fn=30);
  translate([  22,-13,22])rotate([0,0,0])cylinder(h=80, r=3.5,$fn=30);
  translate([ -22, 13,22])rotate([0,0,0])cylinder(h=80, r=3.5,$fn=30);
  translate([  22, 13,22])rotate([0,0,0])cylinder(h=80, r=3.5,$fn=30);
}

