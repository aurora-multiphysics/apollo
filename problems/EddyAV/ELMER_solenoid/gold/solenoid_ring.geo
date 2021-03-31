Mesh.Algorithm3D = 4; // 3D mesh algorithm (1=Delaunay, 4=Frontal, 5=Frontal Delaunay, 6=Frontal Hex, 7=MMG3D, 9=R-tree)

// Parameters
a = .25;	// Solenoid inner radius
b = .3;		// Solenoid outer radius
l = 1;		// Solenoid length

ra = .35;	// Ring inner radius
rb = .40;	// Ring outer radius
rl = .05;	// Ring "height"

c = 3;		// Diameter/Height of computational domain
p = 0.1;
q = 0.5;

// Inner solenoid circle points and curves
Point(1) = {0,0,-l/2,p};
Point(2) = {a,0,-l/2,p};
Point(3) = {0,a,-l/2,p};
Point(4) = {-a,0,-l/2,p};
Point(5) = {0,-a,-l/2,p};
Circle(1) = {2,1,3};
Circle(2) = {3,1,4};
Circle(3) = {4,1,5};
Circle(4) = {5,1,2};
Line Loop(1) = {1,2,3,4};

// Outer solenoid circle points and curves
Point(11) = {0,0,-l/2,p};
Point(12) = {b,0,-l/2,p};
Point(13) = {0,b,-l/2,p};
Point(14) = {-b,0,-l/2,p};
Point(15) = {0,-b,-l/2,p};
Circle(11) = {12,11,13};
Circle(12) = {13,11,14};
Circle(13) = {14,11,15};
Circle(14) = {15,11,12};
Line Loop(2) = {11,12,13,14};

// Inner ring circle points and curves
Point(21) = {0,0,-rl/2,p};
Point(22) = {ra,0,-rl/2,p};
Point(23) = {0,ra,-rl/2,p};
Point(24) = {-ra,0,-rl/2,p};
Point(25) = {0,-ra,-rl/2,p};
Circle(21) = {22,21,23};
Circle(22) = {23,21,24};
Circle(23) = {24,21,25};
Circle(24) = {25,21,22};
Line Loop(3) = {21,22,23,24};

// Outer ring circle points and curves
Point(31) = {0,0,-rl/2,p};
Point(32) = {rb,0,-rl/2,p};
Point(33) = {0,rb,-rl/2,p};
Point(34) = {-rb,0,-rl/2,p};
Point(35) = {0,-rb,-rl/2,p};
Circle(31) = {32,31,33};
Circle(32) = {33,31,34};
Circle(33) = {34,31,35};
Circle(34) = {35,31,32};
Line Loop(4) = {31,32,33,34};

// Create Solenoid
Plane Surface(1) = {2,1};
sol = Extrude {0,0,l} {Surface{1};};
sol_boundary[] = Boundary {Volume{sol[1]};};

// Create Ring
Plane Surface(2) = {4,3};
ring = Extrude {0,0,rl} {Surface{2};};
ring_boundary[] = Boundary {Volume{ring[1]};};

// Create Bounding Region

Point(201) = {c,c,c,q};
Point(202) = {-c,c,c,q};
Point(203) = {-c,-c,c,q};
Point(204) = {c,-c,c,q};
Point(211) = {c,c,-c,q};
Point(212) = {-c,c,-c,q};
Point(213) = {-c,-c,-c,q};
Point(214) = {c,-c,-c,q};
Line(201) = {201,202};
Line(202) = {202,203};
Line(203) = {203,204};
Line(204) = {204,201};
Line(211) = {211,212};
Line(212) = {212,213};
Line(213) = {213,214};
Line(214) = {214,211};
Line(221) = {201,211};
Line(222) = {202,212};
Line(223) = {203,213};
Line(224) = {204,214};

Line Loop(200) = {201,202,203,204};		Plane Surface(200) = {200};
Line Loop(201) = {-211,-214,-213,-212};		Plane Surface(201) = {201};
Line Loop(202) = {221,211,-222,-201};		Plane Surface(202) = {202};
Line Loop(203) = {222,212,-223,-202};		Plane Surface(203) = {203};
Line Loop(204) = {223,213,-224,-203};		Plane Surface(204) = {204};
Line Loop(205) = {224,214,-221,-204};		Plane Surface(205) = {205};

Surface Loop(200) = {200,201,202,203,204,205};
Surface Loop(201) = {sol_boundary[]};
Surface Loop(202) = {ring_boundary[]};
Volume(200) = {200,201,202};


Physical Volume(1) = {sol[1]};	// Solenoid
Physical Volume(2) = {200};		// Air
Physical Volume(3) = {ring[1]};	    // Ring

Physical Surface(21) = {200};
Physical Surface(22) = {201};
Physical Surface(23) = {202};
Physical Surface(24) = {203};
Physical Surface(25) = {204};
Physical Surface(26) = {205};
