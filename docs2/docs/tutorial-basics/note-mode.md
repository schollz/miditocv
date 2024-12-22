---
sidebar_position: 1
headerColor: "red"
---

# Note

The **NOTE** mode is distinguished by the white LED: <span class='white-circle'>⬤</span>

The output voltage quantization is configurable in the editor, the default quantization is the resolution of the knob (12-bit) scaled by the output range (minimum resolution is 0.004v).

The slew rate is configurable in the editor, the default slew rate is 12v/s.  the slew can also be configured by holding shift and turning the knob.

#
Normally, the output voltage is controlled by the midi note number. the output voltage is calculated as `(note - root) / 12` where `note` is the midi note number and `root` is the root note number. the root note number is configurable in the editor, the default root note number is 69 (middle A).

the output voltage quantization is configurable in the editor.

the MIDI channel is configurable in the editor, the default MIDI channel is 0 (any).

the knob can be used to manually change the current pitch, similar to [manual mode](#manual).

The Manual mode lets you change the voltage using the knob.

- `src/pages/index.js` → `localhost:3000/`
- `src/pages/foo.md` → `localhost:3000/foo`
- `src/pages/foo/bar.js` → `localhost:3000/foo/bar`

## Create your first React Page

Create a file at `src/pages/my-react-page.js`:

```jsx title="src/pages/my-react-page.js"
import React from "react";
import Layout from "@theme/Layout";

export default function MyReactPage() {
  return (
    <Layout>
      <h1>My React page</h1>
      <p>This is a React page</p>
    </Layout>
  );
}
```

```lua title="src/pages/my-react-page.js"
-- comment
print("hello world")
```

A new page is now available at [http://localhost:3000/my-react-page](http://localhost:3000/my-react-page).

## Create your first Markdown Page

Create a file at `src/pages/my-markdown-page.md`:

```mdx title="src/pages/my-markdown-page.md"
# My Markdown page

This is a Markdown page
```

A new page is now available at [http://localhost:3000/my-markdown-page](http://localhost:3000/my-markdown-page).
