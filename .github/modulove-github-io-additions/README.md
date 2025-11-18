# MOD2 Download Page for modulove.github.io

This directory contains files to be added to the `modulove.github.io` repository to create a download page for MOD2 firmware.

## Files Included

- **mod2-download-page.html** - Complete download page with modal popups for Patreon-exclusive firmware

## Installation Instructions

### Option 1: Add as a dedicated page

1. Clone or navigate to your `modulove.github.io` repository
2. Copy `mod2-download-page.html` to the root or a `mod2` subdirectory
3. Rename it to `index.html` if using a subdirectory, or give it a descriptive name like `mod2.html`
4. Commit and push:
   ```bash
   git add mod2-download-page.html
   git commit -m "Add MOD2 firmware download page"
   git push
   ```

5. Access the page at:
   - `https://modulove.github.io/mod2-download-page.html` (if in root)
   - `https://modulove.github.io/mod2/` (if in subdirectory as index.html)
   - `https://dl.modulove.io/mod2/` (if CNAME is configured)

### Option 2: Integrate into existing site

If you have an existing site, you can:

1. Extract the CSS from the `<style>` section and add to your main stylesheet
2. Extract the HTML structure for the firmware grid
3. Extract the modal HTML and JavaScript
4. Customize to match your site's design

## Features

### Visual Design
- Dark theme matching modern web aesthetics
- Responsive grid layout
- Card-based design for each firmware
- Hover effects for better UX

### Firmware Cards
- **7 Open-Source Firmware**: Direct download buttons
  - BRAIDS - Macro oscillator
  - CLAP - TR-808 clap
  - CLAVES - Percussion voice
  - FM_DRUM - FM percussion
  - HIHAT - Noise hi-hat
  - KICK - Kick drum
  - VCO - Voltage controlled oscillator

- **2 Patreon-Exclusive Firmware**: Modal popups with information
  - SAMPLE - 18-slot sample player
  - BREAK BEATS - Amen Break & Think Break

### Modal Popups
When users click on Patreon-exclusive firmware:
- Modal appears with description
- Explains why it's Patreon-exclusive (sample data required)
- "Visit Patreon" button opens Hagiwo's Patreon post in new tab
- "Cancel" button closes modal
- Click outside modal or press Escape to close

### Responsive Features
- Mobile-friendly design
- Touch-friendly buttons
- Adapts to different screen sizes

## Customization

### Change Colors
Edit the CSS variables in the `<style>` section:

```css
/* Primary color (downloads) */
background: #0066ff;

/* Patreon color */
background: #ff424d;

/* Background colors */
background: #0a0a0a;  /* Page background */
background: #1a1a1a;  /* Card background */
```

### Add More Firmware
Copy the firmware card structure:

```html
<div class="firmware-card" onclick="window.location.href='releases/MOD2_newfw.uf2'">
    <div class="firmware-name">NEW FIRMWARE</div>
    <div class="firmware-description">Description here</div>
    <a href="releases/MOD2_newfw.uf2" class="download-btn">Download</a>
</div>
```

### Add More Patreon Firmware
1. Copy the patreon firmware card
2. Create a new modal
3. Add the JavaScript function

## File Structure

After installation, your repository should have:

```
modulove.github.io/
├── releases/
│   ├── MOD2_braids.uf2
│   ├── MOD2_clap.uf2
│   ├── MOD2_claves.uf2
│   ├── MOD2_fm_drum.uf2
│   ├── MOD2_hihat.uf2
│   ├── MOD2_kick.uf2
│   └── MOD2_vco.uf2
└── mod2-download-page.html (or mod2/index.html)
```

## Testing

After deployment:

1. Visit the page URL
2. Test direct downloads (should download .uf2 files)
3. Click Patreon-exclusive firmware
4. Verify modal appears
5. Test "Visit Patreon" button opens correct URL
6. Test modal closing (X, Cancel, click outside, Escape key)
7. Test on mobile device for responsiveness

## Browser Compatibility

- Chrome/Edge: ✅ Full support
- Firefox: ✅ Full support
- Safari: ✅ Full support
- Mobile browsers: ✅ Full support

## Patreon URLs

The page links to:
- Sample Player: https://www.patreon.com/posts/code-for-mod2-131363551
- Break Beats: https://www.patreon.com/posts/code-for-mod2-133952127

Make sure these URLs remain valid or update them in the HTML.

## Accessibility

The page includes:
- Semantic HTML
- Keyboard navigation support (Tab, Enter, Escape)
- Clear visual hierarchy
- Readable font sizes
- High contrast colors

## Future Enhancements

Consider adding:
- Firmware version numbers
- Release dates
- Changelog links
- Video tutorials
- Installation troubleshooting
- Search/filter functionality
- Download statistics

## Support

For issues with:
- **Download page**: Update in modulove.github.io repository
- **Firmware builds**: Check MOD2 repository GitHub Actions
- **Deployment**: See MOD2/.github/DEPLOYMENT.md

## Credits

- Hardware Design: Hagiwo
- Firmware: Various contributors (see individual firmware READMEs)
- Download Page: MOD2 Workshop project
