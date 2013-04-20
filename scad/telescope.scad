
include <MCAD/stepper.scad>
include <isothread.scad>

thickness = 2;
cut_margin = 6;
cut_width = 6;
radius = 5;
margin = 1;

module frame_back(width, height, thickness, lingueta) {
  translate([-width/2, -height/2, -thickness/2]) {
    union() {
      difference() {
        cube([width, height, thickness]);
        translate([(width - cut_width)/2, cut_margin, -0.1]) {
          cube([cut_width, height - cut_margin * 2, thickness + 0.2]);
        }
      }
      if (lingueta) {
        translate([(width - cut_width)/2, cut_margin, -thickness*1.5]) {
          cube([cut_width, thickness, thickness * 2.5]);
        }
      }
    }
  }
}

module frame_cavity(length, height, thickness) {
  translate([radius+0.1,-length/2+margin,height/2]) {
    rotate(a=90, v=[-1,0,0]) {
      cylinder(h=length-margin*2, r=radius);
    }
  }
}

module frame_side(length, height, thickness, margin) {
  difference() {
    translate([-thickness/2,-length/2,0]) {
      cube([thickness, length, height]);
    }
    frame_cavity(length, height, thickness);
  }
}

module frame_side_cavities(length, height, thickness) {
  difference() {
    frame_side(length, height, thickness);
    mirror([1,0,0]) {
      frame_cavity(length, height, thickness);
    }
  }
}

module frame_lock(length, height, thickness) {
  translate([thickness/3,length/2-margin,height/2]) {
    rotate(a=90, v=[-1,0,0]) {
      cylinder(h=margin,r=radius);
    }
  }
}

module frame_side_top_lock(length, height, thickness) {
  union() {
    frame_side(length, height, thickness);
    frame_lock(length, height, thickness);
  }
}

module frame_side_locks(length, height, thickness) {
  union() {
    frame_side_cavities(length, height, thickness);
    frame_lock(length, height, thickness);
    mirror([1,0,0]) {
      mirror([0,1,0]) {
        frame_lock(length, height, thickness);
      }
    }
  }
}

module frame_sameside_lock(length, height, thickness) {
  mirror([1,0,0]) {
    union() {
      frame_side(length, height, thickness);
      mirror([0,1,0]) {
        frame_lock(length, height, thickness);
      }
    }
  }
}

module frame_main(width, height, depth, thickness) {
  union() {
    translate([width/2-thickness*1.5,0,0]) {
      frame_back(width, height, thickness, 0);
    }
    frame_side_top_lock(height, depth, thickness*3);
    translate([width-thickness*3,0,0]) {
      mirror([-1,0,0]) {
        frame_side_top_lock(height, depth, thickness*3);
      }
    }
  }
}

module frame_middle(width, height, depth, thickness) {
  union() {
    translate([width/2-thickness*1.5,0,0]) {
      frame_back(width, height, thickness, 1);
    }
    frame_side_locks(height, depth, thickness*3);
    translate([width-thickness*3,0,0]) {
      mirror([-1,0,0]) {
        frame_side_locks(height, depth, thickness*3);
      }
    }
  }
}

module frame_external(width, height, depth, thickness) {
  union() {
    translate([width/2-thickness*1.5,0,0]) {
      frame_back(width, height, thickness, 1);
    }
    frame_sameside_lock(height, depth, thickness*3);
    translate([width-thickness*3,0,0]) {
      mirror([-1,0,0]) {
        frame_sameside_lock(height, depth, thickness*3);
      }
    }
    translate([-thickness*1.5, height/2-20, depth-thickness]) {
      difference() {
        cube([width, 20, thickness]);
        translate([width/2,10,-0.1]) cylinder(5,5.2,5);
      }    
    }
  }
}

module bearing(length, depth, offset) {
  for(i = [0:1:(length-offset)/radius/2-2]) {
    translate([thickness*2,-length/2+offset+i*radius*2+10,depth/2]) {
      sphere(radius-1);
    }
  }
}

module slider(width, height, depth, offset) {
  union() {
    frame_main(width, height, depth, thickness, 1);
    bearing(height, depth, offset);
    translate([width-thickness*7,0,0]) {
      bearing(height, depth, offset);
    }
    translate([8,thickness+0.4+offset,thickness+0.4]) {
      frame_middle(width-16, height, depth-5, thickness, 1);
      bearing(height, depth, offset);
      translate([width-16-thickness*7,0,0]) {
        bearing(height, depth, offset);
      }
    }
    translate([16,thickness*2+0.8+offset*2,thickness*2+0.8]) {
      frame_external(width-32, height, depth-10, thickness, 1);
    }
  }
}

module sliders_box(width, height, depth, distance, offset) {
  slider(width, height, depth, offset);
  translate([0, 0, distance+10]) {
    mirror([0,0,1]) {
      slider(width, height, depth, offset);
    }
  }
  translate([distance-width, 0, 0]) {
    slider(width, height, depth, offset);
    translate([0, 0, distance+10]) {
      mirror([0,0,1]) {
        slider(width, height, depth, offset);
      }
    }
  }
}

module platform(width, height, depth, distance, offset) {
  sliders_box(width, height, depth, distance, offset);
  translate([0,height/2-thickness*6+offset*2,depth+5]) {
    cube([distance,thickness*6,distance-depth*2]);
  }
  translate([-30,height/2-thickness*6+offset*2,distance/2-15]) {
    difference() {
      cube([30, thickness*6, 30]);
      translate([15,-0.1,15]) rotate([-90,0,0]) cylinder(thickness*6+0.2, 10, 10);
    }
    translate([15,-0.1,15]) rotate([-90,0,0]) hex_nut(12,thickness*6+0.2);
  }
  translate([width/2, height/2-thickness*3+offset*2, depth/2]) {
    cylinder(20, thickness*3, thickness*3);
  }
  translate([width/2, height/2-thickness*3+offset*2, distance-depth+thickness]) {
    cylinder(20, thickness*3, thickness*3);
  }
  translate([distance-width/2, height/2-thickness*3+offset*2, depth/2]) {
    cylinder(20, thickness*3, thickness*3);
  }
  translate([distance-width/2, height/2-thickness*3+offset*2, distance-depth+thickness]) {
    cylinder(20, thickness*3, thickness*3);
  }
  rotate([90,0,0]) {
    translate([-15,distance/2,height/2-54]) {
      motor(Nema23, NemaMedium, dualAxis=false);
    }
    translate([-15,distance/2,-height*2+height/2-70]) {
      thread_out(12,height*2);
    }
  }
}

platform(60, 200, 25, 200, 130*sin(180*$t));

