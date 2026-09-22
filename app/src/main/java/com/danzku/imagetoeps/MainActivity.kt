package com.danzku.imagetoeps

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.widget.*
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private lateinit var imageView: ImageView
    private lateinit var status: TextView
    private var bitmap: Bitmap? = null

    private external fun nativeToEps(
        pixels: IntArray,
        width: Int,
        height: Int,
        threshold: Int,
        cell: Int,
        outPath: String
    ): String

    private val picker = registerForActivityResult(
        ActivityResultContracts.GetContent()
    ) { uri ->
        if (uri != null) loadImage(uri)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("imagetoeps")
        buildUi()
    }

    private fun buildUi() {
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(24, 24, 24, 24)
        }

        root.addView(TextView(this).apply {
            text = "Image → EPS"
            textSize = 26f
            setPadding(0, 0, 0, 8)
        })

        root.addView(TextView(this).apply {
            text = "Native C++ vector engine • OpenGL ES available"
            setPadding(0, 0, 0, 12)
        })

        imageView = ImageView(this).apply {
            adjustViewBounds = true
            scaleType = ImageView.ScaleType.FIT_CENTER
            setBackgroundColor(0xFFEFEFEF.toInt())
        }
        root.addView(imageView, LinearLayout.LayoutParams(-1, 0, 1f))

        root.addView(TextView(this).apply { text = "Threshold" })
        val threshold = SeekBar(this).apply {
            max = 255
            progress = 150
        }
        root.addView(threshold)

        root.addView(TextView(this).apply { text = "Detail / cell size (lower = more detail)" })
        val detail = SeekBar(this).apply {
            max = 31
            progress = 3
        }
        root.addView(detail)

        val row = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL }

        row.addView(Button(this).apply {
            text = "Choose image"
            setOnClickListener { picker.launch("image/*") }
        }, LinearLayout.LayoutParams(0, -2, 1f))

        row.addView(Button(this).apply {
            text = "Export EPS"
            setOnClickListener {
                bitmap?.let {
                    exportEps(it, threshold.progress, detail.progress + 1)
                } ?: run { status.text = "Choose an image first." }
            }
        }, LinearLayout.LayoutParams(0, -2, 1f))

        root.addView(row)

        status = TextView(this).apply {
            text = "Ready."
            setPadding(0, 12, 0, 0)
        }
        root.addView(status)

        setContentView(root)
    }

    private fun loadImage(uri: Uri) {
        try {
            contentResolver.openInputStream(uri).use { input ->
                val decoded = BitmapFactory.decodeStream(input)
                if (decoded != null) {
                    bitmap = decoded.copy(Bitmap.Config.ARGB_8888, false)
                    imageView.setImageBitmap(bitmap)
                    status.text = "Loaded ${decoded.width} × ${decoded.height}"
                }
            }
        } catch (e: Exception) {
            status.text = "Load error: ${e.message}"
        }
    }

    private fun exportEps(source: Bitmap, threshold: Int, cell: Int) {
        status.text = "Vectorizing…"
        Thread {
            try {
                val w = source.width
                val h = source.height
                val pixels = IntArray(w * h)
                source.getPixels(pixels, 0, w, 0, 0, w, h)

                val out = java.io.File(
                    getExternalFilesDir(null),
                    "image_${System.currentTimeMillis()}.eps"
                )

                val result = nativeToEps(
                    pixels, w, h, threshold, cell, out.absolutePath
                )

                runOnUiThread { status.text = "EPS saved:\n$result" }
            } catch (e: Exception) {
                runOnUiThread { status.text = "Export error: ${e.message}" }
            }
        }.start()
    }
}
