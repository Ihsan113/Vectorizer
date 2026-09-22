package com.danzku.imagetoeps

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.widget.*
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private lateinit var preview: ImageView
    private lateinit var status: TextView
    private var bitmap: Bitmap? = null

    private external fun nativeToEps(
        pixels: IntArray,
        width: Int,
        height: Int,
        threshold: Int,
        cell: Int,
        output: String
    ): String

    private val picker = registerForActivityResult(
        ActivityResultContracts.GetContent()
    ) { uri -> uri?.let(::loadImage) }

    override fun onCreate(state: Bundle?) {
        super.onCreate(state)
        System.loadLibrary("imagetoeps")
        createUi()
    }

    private fun createUi() {
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(24,24,24,24)
        }

        root.addView(TextView(this).apply {
            text = "Image → EPS"
            textSize = 26f
        })

        root.addView(TextView(this).apply {
            text = "Native vector engine • GPU pipeline ready for next stage"
            setPadding(0,8,0,12)
        })

        preview = ImageView(this).apply {
            adjustViewBounds = true
            scaleType = ImageView.ScaleType.FIT_CENTER
            setBackgroundColor(0xFFEFEFEF.toInt())
        }
        root.addView(preview, LinearLayout.LayoutParams(-1,0,1f))

        root.addView(TextView(this).apply { text = "Threshold" })
        val threshold = SeekBar(this).apply {
            max=255
            progress=150
        }
        root.addView(threshold)

        root.addView(TextView(this).apply { text = "Detail / cell size" })
        val detail = SeekBar(this).apply {
            max=31
            progress=3
        }
        root.addView(detail)

        val row=LinearLayout(this)
        row.addView(Button(this).apply {
            text="Choose"
            setOnClickListener { picker.launch("image/*") }
        },LinearLayout.LayoutParams(0,-2,1f))
        row.addView(Button(this).apply {
            text="Export EPS"
            setOnClickListener {
                bitmap?.let { export(it,threshold.progress,detail.progress+1) }
                    ?: run { status.text="Choose an image first." }
            }
        },LinearLayout.LayoutParams(0,-2,1f))
        root.addView(row)

        status=TextView(this).apply {
            text="Ready"
            setPadding(0,12,0,0)
        }
        root.addView(status)
        setContentView(root)
    }

    private fun loadImage(uri: Uri) {
        try {
            contentResolver.openInputStream(uri).use {
                val b=BitmapFactory.decodeStream(it)
                if(b!=null){
                    bitmap=b.copy(Bitmap.Config.ARGB_8888,false)
                    preview.setImageBitmap(bitmap)
                    status.text="Loaded ${b.width} × ${b.height}"
                }
            }
        } catch(e:Exception) {
            status.text="Load error: ${e.message}"
        }
    }

    private fun export(source:Bitmap,threshold:Int,cell:Int) {
        status.text="Vectorizing..."
        Thread {
            try {
                val pixels=IntArray(source.width*source.height)
                source.getPixels(pixels,0,source.width,0,0,source.width,source.height)
                val out=java.io.File(
                    getExternalFilesDir(null),
                    "image_${System.currentTimeMillis()}.eps"
                )
                val result=nativeToEps(
                    pixels,source.width,source.height,threshold,cell,out.absolutePath
                )
                runOnUiThread { status.text="EPS saved:\n$result" }
            } catch(e:Exception) {
                runOnUiThread { status.text="Export error: ${e.message}" }
            }
        }.start()
    }
}
