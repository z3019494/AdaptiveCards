package io.adaptivecards.renderer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Shader;
import android.graphics.drawable.BitmapDrawable;
import android.widget.ImageView;
import android.widget.LinearLayout;

import io.adaptivecards.objectmodel.BackgroundImage;
import io.adaptivecards.renderer.http.HttpRequestResult;

public class BackgroundImageLoaderAsync extends GenericImageLoaderAsync
{
    private Context m_context;
    private LinearLayout m_layout;
    private BackgroundImage m_backgroundImageProperties;

    public BackgroundImageLoaderAsync(RenderedAdaptiveCard renderedCard, Context context, LinearLayout layout, String imageBaseUrl, BackgroundImage backgroundImageProperties)
    {
        super(renderedCard, imageBaseUrl);

        m_context = context;
        m_layout = layout;
        m_backgroundImageProperties = backgroundImageProperties;
    }

    @Override
    protected HttpRequestResult<Bitmap> doInBackground(String... args)
    {
        if (args.length == 0)
        {
            return null;
        }
        return loadImage(args[0], m_context);
    }

    void onSuccessfulPostExecute(Bitmap bitmap)
    {
        BitmapDrawable background = new BitmapDrawable(m_context.getResources(), bitmap);

        int height = bitmap.getHeight();
        int width = bitmap.getWidth();
        switch(m_backgroundImageProperties.GetMode())
        {
            case Repeat:
                background.setTileModeXY(Shader.TileMode.REPEAT, Shader.TileMode.REPEAT);
                m_layout.setBackground(background);
                break;
            case RepeatHorizontally:
                background.setTileModeX(Shader.TileMode.REPEAT);
                m_layout.setBackground(background);
                switch (m_backgroundImageProperties.GetVerticalAlignment())
                {
                    case Bottom:
                        // NOPE: background.setBounds(m_layout.getLeft(), m_layout.getBottom() - height, m_layout.getRight(), m_layout.getBottom());
                        // NONONONONO: m_layout.setPadding(m_layout.getLeft(),m_layout.getBottom() - height, m_layout.getRight(), m_layout.getBottom());
                        // NOPE, IGNORED WHEN TILEMODE ENABLED: background.setGravity(Gravity.BOTTOM);
                        ////ImageView imageView = new ImageView(m_context);
                        ////imageView.setImageDrawable(background);
                        //imageView.setMinimumWidth(m_layout.getRight());
                        //imageView.setCropToPadding(false);
                        ////imageView.setScaleType(ImageView.ScaleType.FIT_XY);


                        ////m_layout.addView(imageView);
                        //LinearLayout parentView = (LinearLayout) m_layout.getParent();
                        //parentView.addView(imageView);
                        break;
                    case Center:
                        //background.setBounds(m_layout.getLeft(), (m_layout.getBottom() - height)/2, m_layout.getRight(), (m_layout.getBottom() + height)/2);
                        ////ImageView view = new ImageView(m_context);
                        ////view.setImageDrawable(background);
                        ////LinearLayout parent = (LinearLayout) m_layout.getParent();
                        ////view.setX(parent.getLeft());
                        ////view.setY((m_layout.getBottom() - height)/2);
                        ////parent.addView(view);
                        break;
                    case Top:
                    default:
                        ////background.setBounds(m_layout.getLeft(), m_layout.getTop(), m_layout.getRight(), m_layout.getTop() + height);
                        break;
                }
                break;
            case RepeatVertically:
                background.setTileModeY(Shader.TileMode.REPEAT);
                m_layout.setBackground(background);
                ////background.setBounds(m_layout.getLeft(), m_layout.getTop(), width, m_layout.getLeft() + m_layout.getBottom());
                break;
            case Stretch:
            default:
                m_layout.setBackground(background);
                break;
        }
        m_layout.bringChildToFront(m_layout.getChildAt(0));
    }
}
