
l = 40.0;
b = 57.5;
d = 4.0;


bout = b + 2*d;

difference(){
  hull() {
    translate([-bout/2,-l/2,-d])cube([bout,l,d]);
    translate([-bout/2,0,30])rotate([0,90,0])cylinder(h=bout, r=10.5,$fn=30);
  }
 
  translate([-bout/2-1,0,30])rotate([0,90,0])cylinder(h=20, r=2.0,$fn=30);
  translate([ bout/2+1-20,0,30])rotate([0,90,0])cylinder(h=20, r=1.5,$fn=30);
  
  translate([ bout/2+1-20,0,30+7.5])rotate([0,90,0])cylinder(h=20, r=1,$fn=30);
  translate([ bout/2+1-20,+7.5,30])rotate([0,90,0])cylinder(h=20, r=1,$fn=30);
  translate([ bout/2+1-20,-7.5,30])rotate([0,90,0])cylinder(h=20, r=1,$fn=30);
  translate([ bout/2+1-20,0,30-7.5])rotate([0,90,0])cylinder(h=20, r=1,$fn=30);
  
  translate([-b/2,-l/2,0])cube([b,l,60]);
  
  translate([ 0,0,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ 0,10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ 0,-10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  
  translate([ -15,0,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ -15,10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ -15,-10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  
  translate([ 15,0,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ 15,10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  translate([ 15,-10,-5])rotate([0,0,0])cylinder(h=20, r=1.5,$fn=30);
  
}

