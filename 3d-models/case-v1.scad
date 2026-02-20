// case-v1.scad - Zigbee Wekker Behuizing v1
// Parametrisch ontwerp voor ESP32-C6 + SSD1306 OLED + Buzzer + Knop
//
// Afdrukken zonder supports:
//   - Body en deksel apart afdrukken
//   - Body: flat onderkant naar beneden
//   - Deksel: flat binnenkant naar beneden
//
// Toleranties: 0.2 mm voor snap-fit verbindingen
// Aanbevolen instellingen: 0.2mm laagdikte, 20% vulling, 2 omtreklijnen

// ============================================================
// Parameters - Pas hier aan naar wens
// ============================================================

// Buitenafmetingen behuizing
case_width      = 100;   // mm breedte (X)
case_depth      = 75;    // mm diepte (Y)
case_height     = 40;    // mm hoogte totaal (zonder deksel)
wall_thickness  = 2.5;   // mm wanddikte

// Display venster (SSD1306 1.3" 128x64)
display_w       = 35;    // mm breedte zichtbaar gedeelte
display_h       = 28;    // mm hoogte zichtbaar gedeelte
display_offset_x = 0;    // mm horizontale offset van midden (0 = gecentreerd)
display_offset_y = 5;    // mm van bovenrand deksel

// USB-C poort gat (zijkant)
usbc_w          = 10;    // mm breedte
usbc_h          = 4;     // mm hoogte
usbc_offset_y   = 8;     // mm van onderkant

// Knop gat (bovenkant deksel, rechts)
button_dia      = 19.5;  // mm diameter (19mm knop + 0.5mm speling)
button_offset_x = 30;    // mm van midden naar rechts
button_offset_y = 0;     // mm van midden (0 = gecentreerd Y)

// Buzzer gaten (onderkant of zijkant)
buzzer_hole_dia    = 3;   // mm diameter per gat
buzzer_hole_rows   = 3;   // aantal rijen
buzzer_hole_cols   = 4;   // aantal kolommen
buzzer_hole_pitch  = 5;   // mm tussenruimte gaten

// Montage pinnen (voor ESP32-C6 module)
post_dia        = 4;     // mm buitendiameter
post_hole_dia   = 2.1;   // mm gat voor M2 schroef
post_height     = 5;     // mm hoogte paal
// ESP32-C6 XIAO montagegaten (21x17.5 mm, gaatjes op hoeken)
mount_spacing_x = 16.5;
mount_spacing_y = 13.0;
mount_offset_x  = -10;   // positie van ESP32 module in behuizing
mount_offset_y  = 5;

// Snap-fit clips
snap_count      = 3;     // aantal clips per lange zijkant
snap_width      = 8;     // mm breedte per clip
snap_depth      = 1.5;   // mm insprong snap
snap_height     = 3;     // mm hoogte clip

// Kabelgeleiding
cable_channel_w = 6;
cable_channel_h = 3;

// Tolerantie voor passen
fit_tol         = 0.2;

// ============================================================
// Afgeleid
// ============================================================

inner_w = case_width  - 2 * wall_thickness;
inner_d = case_depth  - 2 * wall_thickness;

// ============================================================
// Modules
// ============================================================

// Afgeronde rechthoek (2D)
module rounded_rect(w, d, r) {
    offset(r) offset(-r) square([w, d], center=true);
}

// Afgeronde doos (3D)
module rounded_box(w, d, h, r=2) {
    linear_extrude(h) rounded_rect(w, d, r);
}

// Holle doos (wanden + bodem)
module hollow_box(w, d, h, wall, r=2) {
    difference() {
        rounded_box(w, d, h, r);
        translate([0, 0, wall])
            rounded_box(w - 2*wall, d - 2*wall, h, r - wall);
    }
}

// Montagepaal met M2 gat
module mount_post(h) {
    difference() {
        cylinder(d=post_dia, h=h, $fn=16);
        cylinder(d=post_hole_dia, h=h+1, $fn=16);
    }
}

// Snap-fit clip (voor aan de binnenkant van de zijwand)
module snap_clip() {
    hull() {
        cube([snap_width, snap_depth, snap_height], center=true);
        translate([0, snap_depth/2, snap_height/2])
            cube([snap_width, 0.1, 0.1], center=true);
    }
}

// Buzzer gaten patroon
module buzzer_holes(rows, cols, dia, pitch) {
    for (r = [0:rows-1]) {
        for (c = [0:cols-1]) {
            translate([
                (c - (cols-1)/2) * pitch,
                (r - (rows-1)/2) * pitch,
                0
            ])
            cylinder(d=dia, h=wall_thickness*3, center=true, $fn=12);
        }
    }
}

// ============================================================
// Onderdeel 1: Behuizing Body (onderstuk)
// ============================================================
module case_body() {
    difference() {
        // Buitenwanden + bodem
        hollow_box(case_width, case_depth, case_height, wall_thickness);

        // USB-C poort gat (linkerzijkant)
        translate([
            -case_width/2 - 1,
            0,
            wall_thickness + usbc_offset_y + usbc_h/2
        ])
        cube([wall_thickness + 2, usbc_w, usbc_h], center=true);

        // Kabelgeleiding aan onderkant (optioneel)
        translate([0, case_depth/2 - wall_thickness/2, wall_thickness/2])
        cube([cable_channel_w, wall_thickness*2, cable_channel_h], center=true);
    }

    // Snap-fit clips aan de binnenzijde (voor deksel)
    clip_z = case_height - snap_height/2 - 1;

    // Lange zijden (voor/achter)
    for (side = [-1, 1]) {
        for (i = [0:snap_count-1]) {
            x_pos = (i - (snap_count-1)/2) * (inner_w / snap_count);
            translate([x_pos, side * (case_depth/2 - wall_thickness - snap_depth/2), clip_z])
            rotate([0, 0, side == 1 ? 0 : 180])
            snap_clip();
        }
    }

    // Montagepalen voor ESP32-C6
    for (dx = [-1, 1]) {
        for (dy = [-1, 1]) {
            translate([
                mount_offset_x + dx * mount_spacing_x/2,
                mount_offset_y + dy * mount_spacing_y/2,
                wall_thickness
            ])
            mount_post(post_height);
        }
    }

    // Montagepalen voor OLED display (posities afhankelijk van module)
    // OLED module typisch 38x38 mm met gaten op 32x32 mm
    for (dx = [-1, 1]) {
        for (dy = [-1, 1]) {
            translate([
                display_offset_x + dx * 16,
                -case_depth/2 + wall_thickness + 20 + dy * 16,
                wall_thickness
            ])
            mount_post(post_height);
        }
    }
}

// ============================================================
// Onderdeel 2: Deksel (bovenstuk)
// ============================================================
module case_lid() {
    lid_h = wall_thickness + 3; // dikke deksel voor stevigheid

    difference() {
        union() {
            // Dekselplaat
            rounded_box(case_width, case_depth, lid_h);

            // Rand die in de body past (met fit tolerantie)
            translate([0, 0, -3])
            difference() {
                rounded_box(
                    case_width - 2*(wall_thickness + fit_tol),
                    case_depth - 2*(wall_thickness + fit_tol),
                    4
                );
                rounded_box(
                    case_width - 4*wall_thickness - 2*fit_tol - snap_depth*2,
                    case_depth - 4*wall_thickness - 2*fit_tol - snap_depth*2,
                    4.5
                );
            }
        }

        // Display venster uitsparing
        translate([display_offset_x, case_depth/2 - display_offset_y - display_h/2, -1])
        cube([display_w, display_h, lid_h+2], center=true);

        // Knop gat
        translate([button_offset_x, button_offset_y, -1])
        cylinder(d=button_dia, h=lid_h+2, $fn=32);

        // Snap-fit groeven (voor de clips van de body)
        for (side = [-1, 1]) {
            for (i = [0:snap_count-1]) {
                x_pos = (i - (snap_count-1)/2) * (inner_w / snap_count);
                translate([
                    x_pos,
                    side * (case_depth/2 - wall_thickness - snap_depth),
                    -3 - snap_height/2
                ])
                cube([snap_width + fit_tol, snap_depth + fit_tol, snap_height + 1], center=true);
            }
        }
    }
}

// ============================================================
// Renderen
// Gebruik $fn=32 of hoger voor smoother export
// ============================================================

$fn = 32;

// Toon body
case_body();

// Toon deksel naast de body (voor preview)
translate([0, 0, case_height + 5])
    case_lid();

// Om alleen de body te exporteren als STL:
// case_body();

// Om alleen de deksel te exporteren als STL:
// case_lid();
